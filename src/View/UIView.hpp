#pragma once

#include "Presenter/UIEvent.hpp"
#include "Scene/Scene.h"

#include "nlohmann/json.hpp"

union SDL_Event;

namespace VGG
{

class UIView
{
  std::shared_ptr<Scene> m_scene;

public:
  using EventListener = std::function<void(UIEventPtr)>;
  using ResourcesType = std::map<std::string, std::vector<char>>;

  UIView()
    : m_scene{ std::make_shared<Scene>() }
  {
  }

  auto scene()
  {
    return m_scene;
  }

  void show(const nlohmann::json& viewModel)
  {
    m_scene->loadFileContent(viewModel);
  }

  void setResouces(ResourcesType resources)
  {
    Scene::ResRepo = std::move(resources);
  }

  void setEventListener(EventListener listener)
  {
    m_event_listener = listener;
  }

  void onEvent(const SDL_Event& evt);

private:
  EventListener m_event_listener;
};

} // namespace VGG