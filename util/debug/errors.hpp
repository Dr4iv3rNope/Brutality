#pragma once
#include <stdexcept>

#include "assert.hpp"

#define UTIL_CHECK_ALLOC(variable) \
	UTIL_ASSERT(variable, \
		"Failed to allocate memory region!\n" \
		"Probably not enough RAM in your PC" \
	)

#define UTIL_THROW_EXCEPTION_EX(exceptionClass, toLog, what) \
	{ UTIL_LOG(toLog); throw exceptionClass(what); }

#define UTIL_RUNTIME_ERROR_EX(toLog, what) \
	UTIL_THROW_EXCEPTION_EX(std::runtime_error, toLog, what)

#define UTIL_RUNTIME_ERROR(what) \
	UTIL_RUNTIME_ERROR_EX(UTIL_XOR(L"[EXCEPTION] "##what), UTIL_XOR(what))
