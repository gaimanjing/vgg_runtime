#pragma once
#include "Application/interface/Event/Event.h"
#include "Scene/VGGLayer.h"
#include "AppScene.h"
#include <queue>

namespace VGG
{

class AppRender__pImpl;
class AppRender : public VLayer
{
  VGG_DECL_IMPL(AppRender)
public:
  AppRender();
  void postEvent(UEvent e, void* userData);
  void sendEvent(UEvent e, void* userData);
  void addAppScene(std::shared_ptr<AppScene> listener);
  void beginFrame();
  void render();
  void endFrame();
  ~AppRender();

protected:
  bool onEvent(UEvent e);
};

} // namespace VGG
