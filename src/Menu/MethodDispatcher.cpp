#include "MethodDispatcher.h"

MethodDispatcher::MethodDispatcher() {
}

void MethodDispatcher::Bind(Awesomium::JSObject& object,
	const Awesomium::WebString& name,
	JSDelegate callback) {
	// We can't bind methods to local JSObjects
	if (object.type() == Awesomium::kJSObjectType_Local)
		return;

	object.SetCustomMethod(name, false);

	ObjectMethodKey key(object.remote_id(), name);
	bound_methods_[key] = callback;
}

void MethodDispatcher::BindWithRetval(Awesomium::JSObject& object,
	const Awesomium::WebString& name,
	JSDelegateWithRetval callback) {
	// We can't bind methods to local JSObjects
	if (object.type() == Awesomium::kJSObjectType_Local)
		return;

	object.SetCustomMethod(name, true);

	ObjectMethodKey key(object.remote_id(), name);
	bound_methods_with_retval_[key] = callback;
}

void MethodDispatcher::OnMethodCall(Awesomium::WebView* caller,
	unsigned int remote_object_id,
	const Awesomium::WebString& method_name,
	const Awesomium::JSArray& args) {
	// Find the method that matches the object id + method name
	std::map<ObjectMethodKey, JSDelegate>::iterator i =
		bound_methods_.find(ObjectMethodKey(remote_object_id, method_name));

	// Call the method
	if (i != bound_methods_.end())
		i->second(caller, args);
}

Awesomium::JSValue MethodDispatcher::OnMethodCallWithReturnValue(Awesomium::WebView* caller,
	unsigned int remote_object_id,
	const Awesomium::WebString& method_name,
	const Awesomium::JSArray& args) {
	// Find the method that matches the object id + method name
	std::map<ObjectMethodKey, JSDelegateWithRetval>::iterator i =
		bound_methods_with_retval_.find(ObjectMethodKey(remote_object_id, method_name));

	// Call the method
	if (i != bound_methods_with_retval_.end())
		return i->second(caller, args);

	return Awesomium::JSValue::Undefined();
}