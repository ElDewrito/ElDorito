#pragma once

#include <stdexcept>

class OpticException : public std::runtime_error {
public:
	OpticException() : std::runtime_error("An unknown exception occured!") { }
	OpticException(std::string msg) : std::runtime_error(msg) { };
};