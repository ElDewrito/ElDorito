#pragma once

#include "js_delegate.hpp"
#include <Awesomium/WebCore.h>
#include <map>

/**
 *
 * Helper class that dispatches JSMethodHandler Events to C++ Callbacks.
 *
 * Here's an example of usage:
 *    MyClass my_class;
 *    MethodDispatcher dispatcher;
 *    my_web_view->set_js_method_handler(&dispatcher);
 *    dispatcher.Bind(js_object,
 *                    WSLit("JSMethodName"),
 *                    JSDelegate(&my_class, &MyClass::CPPMethodName));
 */
class MethodDispatcher : public Awesomium::JSMethodHandler {
 public:
  typedef std::pair<int, Awesomium::WebString> ObjectMethodKey;
  typedef std::map<ObjectMethodKey, JSDelegate> BoundMethodMap;
  typedef std::map<ObjectMethodKey, JSDelegateWithRetval> BoundMethodWithRetvalMap;

  MethodDispatcher();

  /**
   * Bind a C++ Callback to a certain JavaScript method (with no return value).
   * 
   * @param  object  The JavaScript Object.
   * @param  name    The name of the function/method to define on the object.
   * @param  callback  The C++ callback to invoke whenever the method is called
   *                   from JavaScript (wrapped via JSDelegate).
   *
   * @note  Callbacks wrapped via JSDelegate should match this function signature:
   *          void MyCallback(WebView* caller, const JSArray& args);
   */
  void Bind(Awesomium::JSObject& object,
    const Awesomium::WebString& name,
    JSDelegate callback);

  /**
   * Bind a C++ Callback to a certain JavaScript method (with a return value).
   * 
   * @param  object  The JavaScript Object.
   * @param  name    The name of the function/method to define on the object.
   * @param  callback  The C++ callback to invoke whenever the method is called
   *                   from JavaScript (wrapped via JSDelegateWithRetval).
   *
   * @note  Callbacks wrapped via JSDelegateWithRetval should match this function signature:
   *          JSValue MyCallback(WebView* caller, const JSArray& args);
   */
  void BindWithRetval(Awesomium::JSObject& object,
    const Awesomium::WebString& name,
    JSDelegateWithRetval callback);

  // Inherited from JSMethodHandler
  void OnMethodCall(Awesomium::WebView* caller,
                    unsigned int remote_object_id, 
                    const Awesomium::WebString& method_name,
                    const Awesomium::JSArray& args);

  // Inherited from JSMethodHandler
  Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller,
                                      unsigned int remote_object_id,
                                      const Awesomium::WebString& method_name,
                                      const Awesomium::JSArray& args);

protected:
  BoundMethodMap bound_methods_;
  BoundMethodWithRetvalMap bound_methods_with_retval_;
};