#include <algorithm>
#include <filesystem>
#include <any>
#include <nlohmann/json.hpp>
#include <EGL/egl.h>
#include <optional>
#include <sstream>
#include <variant>

#include "Core/FontManager.h"
#include "Log.h"
#include "App.hpp"

#include "EGLRuntime.h"
#include "include/core/SkData.h"
#include "encode/SkPngEncoder.h"
#include "Scene/Scene.h"
#include "Core/PaintNode.h"

using namespace VGG;
using namespace std;

const char* get_egl_error_info(EGLint error)
{
  switch (error)
  {
    case EGL_NOT_INITIALIZED:
      return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:
      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:
      return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:
      return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONTEXT:
      return "EGL_BAD_CONTEXT";
    case EGL_BAD_CONFIG:
      return "EGL_BAD_CONFIG";
    case EGL_BAD_CURRENT_SURFACE:
      return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:
      return "EGL_BAD_DISPLAY";
    case EGL_BAD_SURFACE:
      return "EGL_BAD_SURFACE";
    case EGL_BAD_MATCH:
      return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:
      return "EGL_BAD_PARAMETER";
    case EGL_BAD_NATIVE_PIXMAP:
      return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW:
      return "EGL_BAD_NATIVE_WINDOW";
    case EGL_CONTEXT_LOST:
      return "EGL_CONTEXT_LOST";
    case EGL_SUCCESS:
      return "NO ERROR";
    default:
      return "UNKNOWN ERROR";
  }
}

#define EGL_CHECK()                                                                                \
  do                                                                                               \
  {                                                                                                \
    auto error = eglGetError();                                                                    \
    if (error != EGL_SUCCESS)                                                                      \
    {                                                                                              \
      WARN("EGL Error: %s", get_egl_error_info(error));                                            \
    }                                                                                              \
  } while (0)

constexpr int opengl_version[] = { 4, 5 };
constexpr int pixel_buffer_width = 800;
constexpr int pixel_buffer_height = 600;

constexpr EGLint config_attribs[] = { EGL_SURFACE_TYPE,
                                      EGL_PBUFFER_BIT,
                                      EGL_BLUE_SIZE,
                                      8,
                                      EGL_GREEN_SIZE,
                                      8,
                                      EGL_RED_SIZE,
                                      8,
                                      EGL_DEPTH_SIZE,
                                      8,
                                      EGL_RENDERABLE_TYPE,
                                      EGL_OPENGL_BIT,
                                      EGL_NONE };

constexpr EGLint context_attris[] = { EGL_CONTEXT_MAJOR_VERSION,
                                      opengl_version[0],
                                      EGL_CONTEXT_MINOR_VERSION,
                                      opengl_version[1],
                                      EGL_CONTEXT_OPENGL_PROFILE_MASK,
                                      EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
                                      EGL_NONE };

class EGLRuntime : public App<EGLRuntime>
{
  EGLContext egl_ctx;
  EGLSurface egl_surface;
  EGLDisplay egl_display;
  EGLConfig egl_config = nullptr;
  std::unordered_map<std::string, std::any> m_properties;

private:
  void resizeEGLSurface(int w, int h)
  {

    EGLint width = w;
    EGLint height = h;

    eglDestroySurface(egl_display, egl_surface);
    EGL_CHECK();
    EGLint pixel_buffer_attribs[] = {
      EGL_WIDTH, w, EGL_HEIGHT, h, EGL_NONE,
    };
    egl_surface = eglCreatePbufferSurface(egl_display, egl_config, pixel_buffer_attribs);
    EGL_CHECK();

    eglBindAPI(EGL_OPENGL_API);
    // Make the context current with the new surface
    makeContextCurrent();
    EGL_CHECK();
  }

public:
  float getDPIScale()
  {
    return 1.0;
  }
  std::optional<AppError> initContext(int w, int h, const std::string& title)
  {
    // DPI::ScaleFactor = get_scale_factor();
    // int winWidth = w * DPI::ScaleFactor;
    // int winHeight = h * DPI::ScaleFactor;
    int winWidth = w;
    int winHeight = h;

    m_properties["window_size"] = std::pair{ winWidth, winHeight };
    m_properties["viewport_size"] = std::pair{ winWidth, winHeight };
    m_properties["app_size"] = std::pair{ winWidth, winHeight };

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY)
    {
      WARN("No default display");
      // try EXT_platform_device, see
      // https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_platform_device.txt
      // egl_display = create_display_from_device();
      return AppError(AppError::Kind::EGLNoDisplayError, "No default display");
    }

    EGLint major = 0, minor = 0;
    eglInitialize(egl_display, &major, &minor);
    INFO("EGL version: %d, %d", major, minor);
    INFO("EGL vendor string: %s", eglQueryString(egl_display, EGL_VENDOR));
    EGL_CHECK();

    // 2. Select an appropriate configuration
    EGLint num_configs = 0;
    eglChooseConfig(egl_display, config_attribs, &egl_config, 1, &num_configs);
    EGL_CHECK();

    int maxSurfaceSize[2];
    if (eglGetConfigAttrib(egl_display, egl_config, EGL_MAX_PBUFFER_WIDTH, maxSurfaceSize) !=
        EGL_TRUE)
    {
      EGL_CHECK();
      return AppError(AppError::Kind::EGLGetAttribError, "get attribute error");
    }
    if (eglGetConfigAttrib(egl_display, egl_config, EGL_MAX_PBUFFER_HEIGHT, maxSurfaceSize + 1) !=
        EGL_TRUE)
    {
      EGL_CHECK();
      return AppError(AppError::Kind::EGLGetAttribError, "get attribute error");
    }

    if (winWidth + 100 > maxSurfaceSize[0] || winHeight + 100 > maxSurfaceSize[1])
    {
      return AppError(AppError::Kind::TextureSizeOutOfRangeError, "Texture size out of range");
    }

    // 3. Create a surface

    EGLint pixel_buffer_attribs[] = {
      EGL_WIDTH, w, EGL_HEIGHT, h, EGL_NONE,
    };
    egl_surface = eglCreatePbufferSurface(egl_display, egl_config, pixel_buffer_attribs);
    EGL_CHECK();

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);
    EGL_CHECK();

    // 5. Create a context and make it current
    egl_ctx = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attris);
    EGL_CHECK();

    return makeContextCurrent();
  }
  std::optional<AppError> makeContextCurrent()
  {
    if (eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_ctx) == EGL_TRUE)
    {
      return nullopt;
    }
    EGL_CHECK();
    return AppError(AppError::Kind::MakeCurrentContextError, "make current failed");
  }

  std::any getProperty(const std::string& name)
  {
    // the properties result doesnt matter in EGL backend
    auto it = m_properties.find(name);
    if (it != m_properties.end())
    {
      return it->second;
    }
    return std::any();
  }

  void setProperty(const std::string& name, std::any value)
  {
    auto it = m_properties.find(name);
    if (it != m_properties.end())
    {
      if (name == "viewport_size")
      {
        auto size = std::any_cast<std::pair<int, int>>(value);
        auto prop = std::any_cast<std::pair<int, int>>(it->second);
        if (size.first != prop.first || size.second != prop.second)
        {
          resizeEGLSurface(prop.first, prop.second);
          updateSkiaEngine();
          resizeSkiaSurface(prop.first, prop.second);
          it->second = value;
        }
      }
    }
  }

  void onInit()
  {
    // do nothing
  }

  void pollEvent()
  {
    // no event need to deal with
  }

  void swapBuffer()
  {
    if (eglSwapBuffers(egl_display, egl_surface) == EGL_FALSE)
    {
      WARN("egl swap buffer failed\n");
    }
  }

  ~EGLRuntime()
  {
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(egl_display, egl_ctx);
    eglDestroySurface(egl_display, egl_surface);
    eglTerminate(egl_display);
    // eglReleaseThread();
  }
};

float calcScaleFactor(float inputWidth,
                      float inputHeight,
                      float maxWidth,
                      float maxHeight,
                      float& outWidth,
                      float& outHeight)
{
  auto widthScale = maxWidth / inputWidth;
  auto heightScale = maxHeight / inputHeight;
  float outputSize[2] = { 0.f, 0.f };
  if (widthScale < heightScale)
  {
    outWidth = maxWidth;
    outHeight = widthScale * inputHeight;
  }
  else
  {
    outWidth = heightScale * inputWidth;
    outHeight = maxHeight;
  }
  return widthScale > heightScale ? heightScale : widthScale;
}

void getMaxSurfaceSize(int resolutionLevel, float* maxSurfaceSize)
{
  switch (resolutionLevel)
  {
    case 0:
    {
      maxSurfaceSize[0] = 2048;
      maxSurfaceSize[1] = 2048;
    }
    break;
    case 1:

    {
      maxSurfaceSize[0] = 2048;
      maxSurfaceSize[1] = 2048;
    }
    break;
    case 2:
    {
      maxSurfaceSize[0] = 4096;
      maxSurfaceSize[1] = 4096;
    }
    break;
    case 3:
    {
      maxSurfaceSize[0] = 8192;
      maxSurfaceSize[1] = 8192;
    }
    break;
    case 4:
    {
      maxSurfaceSize[0] = 16384;
      maxSurfaceSize[1] = 16384;
    }
    break;
    default:
    {
      maxSurfaceSize[0] = 2048;
      maxSurfaceSize[1] = 2048;
    }
  }
}

std::tuple<std::string, std::vector<std::pair<std::string, std::vector<char>>>> render(
  const nlohmann::json& j,
  const std::map<std::string, std::vector<char>>& resources,
  int imageQuality,
  int resolutionLevel,
  const std::string& configFile,
  const std::string& fontCollectionName)
{

  Config::readGlobalConfig(configFile);
  float maxSurfaceSize[2];
  getMaxSurfaceSize(resolutionLevel, maxSurfaceSize);
  std::stringstream ss;
  auto appResult = App<EGLRuntime>::createInstance(maxSurfaceSize[0], maxSurfaceSize[1], 1.0);
  if (const auto error = std::get_if<AppError>(&appResult))
  {
    ss << "create instance failed: " << error->text << std::endl;
    std::string s;
    ss >> s;
    return { s, {} };
  }
  auto v = std::get_if<EGLRuntime*>(&appResult);
  auto app = *v;
  if (!app)
  {
    return { "Failed to create instance", {} };
  }

  FontManager::instance().setDefaultFontManager(fontCollectionName);
  std::vector<std::pair<std::string, std::vector<char>>> res;
  auto scene = std::make_shared<Scene>();
  scene->loadFileContent(j);
  scene->setResRepo(resources);
  auto count = scene->frameCount();
  for (int i = 0; i < count; i++)
  {
    auto b = scene->frame(i)->getBound();
    int w = b.size().x;
    int h = b.size().y;
    scene->setPage(i);
    app->setUseOldRenderer(false);
    app->setScene(scene);
    auto canvas = app->getCanvas();
    if (canvas)
    {
      float actualSize[2];
      auto scale =
        calcScaleFactor(w, h, maxSurfaceSize[0], maxSurfaceSize[1], actualSize[0], actualSize[1]);
      app->setScale(scale);
      app->frame(0);
      if (auto surface = app->getSurface())
      {
        if (auto image = surface->makeImageSnapshot(
              SkIRect::MakeXYWH(0, 0, (int)actualSize[0], (int)actualSize[1])))
        {
          SkPngEncoder::Options opt;
          opt.fZLibLevel = std::max(std::min(9, (100 - imageQuality) / 10), 0);
          if (auto data = SkPngEncoder::Encode(app->getDirectContext(), image.get(), opt))
          {
            res.emplace_back(scene->frame(i)->guid(),
                             std::vector<char>{ data->bytes(), data->bytes() + data->size() });
          }
          else
          {
            ss << "failed to encode image for artboard: " << i + 1 << std::endl;
          }
        }
        else
        {
          ss << "failed to render into image for artboard: " << i + 1 << std::endl;
        }
      }
      else
      {
        ss << "failed to create surface for artboard: " << i + 1 << std::endl;
      }
    }
  }

  App<EGLRuntime>::destoryInstance(app);
  return { std::string{ std::istreambuf_iterator<char>{ ss }, std::istreambuf_iterator<char>{} },
           res };
}