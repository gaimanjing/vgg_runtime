#pragma once

#include "Entry/SDL/EventAPISDLImpl.h"
#include "Event/EventAPI.h"
#include "NewApp.hpp"
#include "Log.h"
#include "EventConvert.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <optional>
#include <any>

using namespace VGGNew;

namespace VGGNew
{
class SDLRuntime : public VGGNew::App<SDLRuntime>
{
  struct SDLState
  {
    SDL_Window* window{ nullptr };
    SDL_GLContext glContext{ nullptr };
  };
  SDLState m_sdlState;
  using Getter = std::function<std::any(void)>;
  using Setter = std::function<void(std::any)>;
  std::unordered_map<std::string, std::pair<Getter, Setter>> m_properties;

  void initPropertyMap()
  {
    m_properties["viewport_size"] = { [this]() -> std::any
                                      {
                                        int dw, dh;
                                        SDL_GL_GetDrawableSize(this->m_sdlState.window, &dw, &dh);
                                        return std::any(std::pair<int, int>(dw, dh));
                                      },
                                      Setter() };
    m_properties["window_size"] = { [this]()
                                    {
                                      int dw, dh;
                                      SDL_GetWindowSize(this->m_sdlState.window, &dw, &dh);
                                      return std::any(std::pair<int, int>(dw, dh));
                                    },
                                    [this](std::any size)
                                    {
                                      auto p = std::any_cast<std::pair<int, int>>(size);
                                      SDL_SetWindowSize(this->m_sdlState.window, p.first, p.second);
                                    } };
    m_properties["app_size"] = { [this]() { return std::pair<int, int>(m_width, m_height); },
                                 [this](std::any size)
                                 {
                                   auto p = std::any_cast<std::pair<int, int>>(size);
                                   m_width = p.first;
                                   m_height = p.second;
                                 } };
  }

public:
  static inline void handleSDLError()
  {
    const char* err = SDL_GetError();
    FAIL("SDL Error: %s", err);
    SDL_ClearError();
  }

#ifdef __linux__
  static double getScaleFactor()
  {
    static constexpr int NVARS = 4;
    static const char* s_vars[NVARS] = {
      "FORCE_SCALE",
      "QT_SCALE_FACTOR",
      "QT_SCREEN_SCALE_FACTOR",
      "GDK_SCALE",
    };
    for (int i = 0; i < NVARS; i++)
    {
      const char* strVal = getenv(s_vars[i]);
      if (strVal)
      {
        double val = atof(strVal);
        if (val >= 1.0)
        {
          return val;
        }
      }
    }
    return 1.0;
  }
#else
  static inline double getScaleFactor()
  {
    return 1.0;
  }
#endif
  std::optional<AppError> initContext(int w, int h)
  {
    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      handleSDLError();
      return AppError(AppError::Kind::RenderEngineError, "sdl init failed");
    }
    SDL_version compileVersion, linkVersion;
    SDL_VERSION(&compileVersion);
    SDL_GetVersion(&linkVersion);
    INFO("SDL version %d.%d.%d (linked %d.%d.%d)",
         compileVersion.major,
         compileVersion.minor,
         compileVersion.patch,
         linkVersion.major,
         linkVersion.minor,
         linkVersion.patch);

    // setup opengl properties
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, appConfig().videoConfig.stencilBit);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, appConfig().videoConfig.multiSample);
    // SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
#ifndef EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
#endif

    // create window
    float f = getScaleFactor();
    int winWidth = w * f;
    int winHeight = h * f;
    SDL_Window* window =
#ifndef EMSCRIPTEN
      SDL_CreateWindow(appName().c_str(),
#else
      SDL_CreateWindow(nullptr,
#endif
                       SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED,
                       winWidth,
                       winHeight,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window)
    {
      handleSDLError();
      return AppError(AppError::Kind::RenderEngineError, "Create Window Failed\n");
    }
    m_width = w;
    m_height = h;
    m_sdlState.window = window;
    // create context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
      handleSDLError();
      return AppError(AppError::Kind::RenderEngineError, "Create Context Failed");
    }

    m_sdlState.glContext = glContext;

    // switch to this context
    auto appResult = makeContextCurrent();
    if (appResult.has_value())
    {
      return appResult;
    }
    INFO("GL_VENDOR: %s", glGetString(GL_VENDOR));
    INFO("GL_RENDERER: %s", glGetString(GL_RENDERER));
    INFO("GL_VERSION: %s", glGetString(GL_VERSION));
    INFO("GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // get device pixel ratio
    //

    initPropertyMap();
    return std::nullopt;
  }

  std::optional<AppError> makeContextCurrent()
  {
    if (SDL_GL_MakeCurrent(m_sdlState.window, m_sdlState.glContext) != 0)
    {
      handleSDLError();
      return AppError(AppError::Kind::MakeCurrentContextError, "Make Current Context Error");
    }
    return std::nullopt;
  }

  std::any getProperty(const std::string& name)
  {
    auto it = m_properties.find(name);
    if (it != m_properties.end())
    {
      return (it->second.first)(); // getter
    }
    return std::any();
  }

  void setProperty(const std::string& name, std::any value)
  {
    auto it = m_properties.find(name);
    if (it != m_properties.end())
    {
      (it->second.second)(value); // setter
    }
  }

  float getDPIScale()
  {
#ifdef VGG_HOST_macOS
    int dw, dh;
    int ww, wh;
    SDL_GL_GetDrawableSize(this->m_sdlState.window, &dw, &dh);
    SDL_GetWindowSize(this->m_sdlState.window, &ww, &wh);
    std::cout << dw << " " << ww << " asfdsafdasf" << std::endl;
    return float(dw) / (float)ww;
#else
    return getScaleFactor();
#endif
  }

  void onInit()
  {
    SDL_GL_SetSwapInterval(0);

    // Reigster SDL event impl
    auto eventAPIImpl = std::make_unique<EventAPISDLImpl>();
    EventManager::registerEventAPI(std::move(eventAPIImpl));
    // SDL_ShowCursor(SDL_DISABLE);
  }

  void pollEvent()
  {
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
      sendEvent(toUEvent(evt));
    }
  }

  void swapBuffer()
  {

    // auto profiler = CappingProfiler::getInstance();

    // // display fps
    // if (profiler)
    // {
    //   SDL_SetWindowTitle(m_sdlState.window, profiler->fpsStr());
    // }

    // swap buffer at last
    SDL_GL_SwapWindow(m_sdlState.window);
  }

  ~SDLRuntime()
  {
    if (m_inited && m_sdlState.glContext)
    {
      // NOTE The failure to delete GL context may be related to multi-thread and is hard
      // to make it right. For simplicity, we can just safely ignore the deletion.
      //
      // SDL_GL_DeleteContext(m_sdlState.glContext);
    }
    if (m_inited && m_sdlState.window)
    {
      SDL_DestroyWindow(m_sdlState.window);
    }
    SDL_Quit();
  }
};
} // namespace VGGNew