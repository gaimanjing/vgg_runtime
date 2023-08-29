#include "Controller.hpp"

#include "Usecase/EditModel.hpp"

#include "Domain/VggExec.hpp"
#include "Domain/RawJsonDocument.hpp"
#include "Domain/SchemaValidJsonDocument.hpp"
// #include "Domain/UndoRedoJsonDocument.hpp"
#include "Domain/Daruma.hpp"
#include "Domain/DarumaContainer.hpp"
#include "Presenter.hpp"
#include "DIContainer.hpp"
#include "Log.h"
#include "Usecase/ModelChanged.hpp"
#include "Usecase/ResizeWindow.hpp"
#include "Usecase/StartRunning.hpp"

#include <cassert>

constexpr auto pseudo_path_edit_view = "::editView";

namespace VGG
{

Controller::Controller(std::shared_ptr<RunLoop> runLoop,
                       std::shared_ptr<Presenter> presenter,
                       RunMode mode)
  : m_run_loop(runLoop)
  , m_presenter(presenter)
  , m_mode(mode)
{
  assert(m_run_loop);
}

bool Controller::start(const std::string& filePath, const char* designDocSchemaFilePath)
{
  initModel(designDocSchemaFilePath);
  auto ret = m_model->load(filePath);
  if (ret)
  {
    start();
  }
  else
  {
    FAIL("#controller, load file failed, %s", filePath.c_str());
  }
  return ret;
}

bool Controller::start(std::vector<char>& buffer, const char* designDocSchemaFilePath)
{
  initModel(designDocSchemaFilePath);
  auto ret = m_model->load(buffer);
  if (ret)
  {
    start();
  }
  else
  {
    FAIL("#controller, load buffer failed");
  }
  return ret;
}

bool Controller::edit(const std::string& filePath)
{
  EditModel edit_model{ m_design_schema_file_path };
  auto daruma_to_edit = edit_model.open(filePath);
  if (daruma_to_edit)
  {
    m_edit_model = daruma_to_edit;
    startEditing();
    return true;
  }
  else
  {
    return false;
  }
}

bool Controller::edit(std::vector<char>& buffer)
{
  EditModel edit_model{ m_design_schema_file_path };
  auto daruma_to_edit = edit_model.open(buffer);
  if (daruma_to_edit)
  {
    m_edit_model = daruma_to_edit;
    startEditing();
    return true;
  }
  else
  {
    return false;
  }
}

void Controller::onResize()
{
  ResizeWindow().onResize(m_model, m_presenter->viewSize());
  if (m_edit_model)
  {
    ResizeWindow().onResize(m_edit_model, m_presenter->editViewSize());
  }
}

void Controller::initModel(const char* designDocSchemaFilePath)
{
  if (designDocSchemaFilePath)
  {
    m_design_schema_file_path.append(designDocSchemaFilePath);
  }

  auto build_design_doc_fn =
    [&, design_schema_file_path = m_design_schema_file_path](const json& designJson)
  {
    auto json_doc_ptr = createJsonDoc();
    json_doc_ptr->setContent(designJson);

    if (!design_schema_file_path.empty())
    {
      SchemaValidJsonDocument::ValidatorPtr design_doc_validator;
      std::ifstream schema_fs(design_schema_file_path);
      json schema = json::parse(schema_fs);
      design_doc_validator.reset(new JsonSchemaValidator);
      design_doc_validator->setRootSchema(schema);

      json_doc_ptr =
        new SchemaValidJsonDocument(JsonDocumentPtr(json_doc_ptr), design_doc_validator);
    }

    return wrapJsonDoc(JsonDocumentPtr(json_doc_ptr));
  };
  // todo, build layout doc
  m_model.reset(new Daruma(build_design_doc_fn));

  DarumaContainer().add(m_model);
}

void Controller::start()
{
  m_presenter->setModel(generateViewModel(m_model, m_presenter->viewSize()));

  observeModelState();
  observeViewEvent();
}

void Controller::startEditing()
{
  m_presenter->setEditModel(generateViewModel(m_edit_model, m_presenter->editViewSize()));

  observeEditModelState();
  observeEditViewEvent();

  DarumaContainer().add(m_edit_model, DarumaContainer::KeyType::Edited);
}

void Controller::observeModelState()
{
  auto weak_this = weak_from_this();
  m_model->getObservable()
    .observe_on(m_run_loop->thread())
    .map(
      [weak_this](VGG::ModelEventPtr event)
      {
        auto shared_this = weak_this.lock();
        if (!shared_this)
        {
          return VGG::ModelEventPtr{};
        }
        // todo, layout
        // todo, layout thread?
        ModelChanged().onChange(shared_this->m_model);
        shared_this->resetViewModel();

        return event;
      })
    .subscribe(m_presenter->getModelObserver());
}

void Controller::observeEditModelState()
{
  auto weak_this = weak_from_this();
  m_edit_model->getObservable()
    .observe_on(m_run_loop->thread())
    .map(
      [weak_this](VGG::ModelEventPtr event)
      {
        auto shared_this = weak_this.lock();
        if (!shared_this)
        {
          return VGG::ModelEventPtr{};
        }
        // todo, layout
        // todo, layout thread?

        ModelChanged().onChange(shared_this->m_edit_model);
        shared_this->resetEditViewModel();

        return event;
      })
    .subscribe(m_presenter->getEditModelObserver());
}

void Controller::observeViewEvent()
{
  auto weak_this = weak_from_this();
  auto observer = rxcpp::make_observer_dynamic<UIEventPtr>(
    [weak_this](UIEventPtr evt)
    {
      auto shared_this = weak_this.lock();
      if (!shared_this)
      {
        return;
      }

      auto listeners_map = shared_this->m_model->getEventListeners(evt->path());
      std::string type = evt->type();
      if (auto it = listeners_map.find(type); it != listeners_map.end())
      {
        for (auto& listener : it->second)
        {
          // todo, evt phase // kCapturingPhase = 1, // kAtTarget = 2, // kBubblingPhase = 3
          // todo, evt PropagationStopped
          shared_this->vggExec()->evalModule(listener, evt);
        }
      }
    });

  m_presenter->getObservable().subscribe(observer);
}

void Controller::observeEditViewEvent()
{
  auto weak_this = weak_from_this();
  auto observer = rxcpp::make_observer_dynamic<UIEventPtr>(
    [weak_this](UIEventPtr evt)
    {
      auto shared_this = weak_this.lock();
      if (!shared_this)
      {
        return;
      }

      auto listeners_map = shared_this->m_model->getEventListeners(pseudo_path_edit_view);
      std::string type = evt->type();
      if (auto it = listeners_map.find(type); it != listeners_map.end())
      {
        for (auto& listener : it->second)
        {
          shared_this->vggExec()->evalModule(listener, evt);
        }
      }
    });

  m_presenter->getEditObservable().subscribe(observer);
}

const std::shared_ptr<VggExec>& Controller::vggExec()
{
  return VGG::DIContainer<std::shared_ptr<VggExec>>::get();
}

JsonDocument* Controller::createJsonDoc()
{
  if (m_mode == RunMode::NormalMode)
  {
    return new RawJsonDocument();
  }
  else
  {
    return nullptr; // new UndoRedoJsonDocument();
  }
}

JsonDocumentPtr Controller::wrapJsonDoc(std::shared_ptr<JsonDocument> jsonDoc)
{
  if (m_mode == RunMode::NormalMode)
  {
    return jsonDoc;
  }
  else
  {
    // todo, wrap with remote doc which can save to server
    return jsonDoc;
  }
}

void Controller::resetViewModel()
{
  // TODO, OPTIMIZE, partial update instead of resetting the whole document
  m_presenter->setModel(generateViewModel(m_model, m_presenter->viewSize()));
}

void Controller::resetEditViewModel()
{
  // TODO, OPTIMIZE, partial update instead of resetting the whole document
  m_presenter->setEditModel(generateViewModel(m_edit_model, m_presenter->editViewSize()));
}

std::shared_ptr<ViewModel> Controller::generateViewModel(std::shared_ptr<Daruma> model,
                                                         Layout::Size size)
{
  StartRunning start_running{ model };
  start_running.layout(size);

  auto view_model = std::make_shared<ViewModel>();
  view_model->model = m_model;
  view_model->designDoc = start_running.designDoc();
  view_model->layoutTree = start_running.layoutTree();

  return view_model;
}

} // namespace VGG