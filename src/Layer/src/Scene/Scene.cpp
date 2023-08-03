#include "Scene/Scene.h"
#include "Reader/Loader.h"
#include "Core/PaintNode.h"
#include "Scene/Renderer.h"
#include "core/SkCanvas.h"
#include <filesystem>
#include "ConfigMananger.h"
#include <fstream>
#include <memory>
#include <string>
namespace VGG
{

ResourceRepo Scene::s_resRepo{};
ObjectTableType Scene::s_objectTable{};
ObjectTableType Scene::s_templateObjectTable{};
InstanceTable Scene::s_instanceTable{};
bool Scene::s_enableDrawDebugBound{ false };

Scene::Scene()
{
}
void Scene::loadFileContent(const std::string& json)
{
  loadFileContent(nlohmann::json::parse(json));
}

void Scene::instantiateTemplates()
{
  for (auto& p : Scene::instanceObjects())
  {
    if (auto node = p.second.first.lock(); node)
    {
      auto& templates = Scene::templateObjectTable();
      if (auto it = templates.find(p.second.second); it != templates.end())
      {
        if (auto master = it->second.lock(); master)
        {
          auto instance = std::static_pointer_cast<PaintNode>(master->cloneRecursive());
          // TODO:: overide properties
          // TODO:: clear transform
          node->addChild(instance);
        }
        else
        {
          ASSERT_MSG(false, "master node is expired");
        }
      }
      else
      {
        DEBUG("symbol master [%s] doesn't exist referenced by [%s]",
              p.second.second.c_str(),
              p.first.c_str());
      }
    }
  }
}

void Scene::loadFileContent(const nlohmann::json& json)
{
  if (json.empty())
    return;
  s_templateObjectTable.clear();
  s_instanceTable.clear();
  page = 0;
  symbolIndex = 0;
  maskDirty = true;

  symbols = NlohmannBuilder::fromSymbolMasters(json);
  for (const auto& s : symbols)
  {
    s_templateObjectTable[s->guid()] = s;
  }
  artboards = NlohmannBuilder::fromArtboard(json);
  instantiateTemplates();
}

void Scene::render(SkCanvas* canvas)
{
  PaintNode* node = nullptr;
  SkiaRenderer r;
  if (!renderSymbol)
  {
    if (!artboards.empty())
    {
      auto board = artboards[page].get();
      preprocessMask(board);
      r.draw(canvas, board);
    }
  }
  else
  {
    if (!symbols.empty())
    {
      node = symbols[symbolIndex].get();
      r.draw(canvas, node);
    }
  }
}

void Scene::preprocessMask(PaintNode* node)
{
  if (maskDirty)
  {
    Scene::s_objectTable = node->preprocessMask();
    // generate each mask for masked node
    maskDirty = false;
  }
}

void Scene::setPage(int num)
{
  if (num >= 0 && num < artboards.size())
  {
    page = num;
    maskDirty = true;
  }
}

void Scene::nextArtboard()
{
  page = (page + 1 >= artboards.size()) ? page : page + 1;
  maskDirty = true;
}

void Scene::preArtboard()
{
  page = (page - 1 > 0) ? page - 1 : 0;
  maskDirty = true;
}

void Scene::nextSymbol()
{
  symbolIndex = (symbolIndex + 1 >= symbols.size()) ? symbolIndex : symbolIndex + 1;
}

void Scene::prevSymbol()
{
  symbolIndex = (symbolIndex - 1 > 0) ? symbolIndex - 1 : 0;
}

} // namespace VGG
