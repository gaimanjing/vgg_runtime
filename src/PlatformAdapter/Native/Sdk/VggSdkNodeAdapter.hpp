#ifndef VGG_SDK_NODE_ADAPTER_HPP
#define VGG_SDK_NODE_ADAPTER_HPP

#include <memory>
#include <vector>

#include "js_native_api.h"

class VggSdk;

class VggSdkNodeAdapter
{
public:
  static void Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

private:
  VggSdkNodeAdapter();
  ~VggSdkNodeAdapter();

  static napi_value New(napi_env env, napi_callback_info info);

  static napi_value GetDesignDocument(napi_env env, napi_callback_info info);

  static napi_value DesignDocumentReplaceAt(napi_env env, napi_callback_info info);
  static napi_value DesignDocumentAddAt(napi_env env, napi_callback_info info);
  static napi_value DesignDocumentDeleteAt(napi_env env, napi_callback_info info);

  static napi_value Undo(napi_env env, napi_callback_info info);
  static napi_value Redo(napi_env env, napi_callback_info info);

  static void GetArgString_(napi_env env, std::string& to_string, napi_value arg);

  static napi_ref constructor;

  napi_env m_env;
  napi_ref m_wrapper;
  std::shared_ptr<VggSdk> m_vggSdk;
};

#endif