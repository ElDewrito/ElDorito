#pragma once

#include <stdexcept>

class HookException : public std::runtime_error {
public:
	HookException() : std::runtime_error("Hooks failed to install") { }
	HookException(std::string msg) : std::runtime_error(msg) { };
};

class IHook {
public:
	virtual ~IHook() {};
	virtual bool install() = 0;
	virtual void uninstall() = 0;
};