#pragma once
#include "logs.hpp"

#include <cstdlib>

#ifdef __UTIL_DISABLE_LOGS
#define __UTIL_DISABLE_ASSERT
#endif

#ifndef __UTIL_DISABLE_ASSERT

#ifdef _DEBUG
#define UTIL_DEBUG_ASSERT(operation) \
	if (!(operation)) { \
		UTIL_XLOG(L"[ASSERTATION FAULT] operation: { "#operation##" }"); \
		__asm { int 3 }; \
		std::abort(); }

#define UTIL_ASSERT(operation, what) \
	if (!(operation)) { \
		UTIL_XLOG(L"[ASSERTATION FAULT] what: "##what); \
		__asm { int 3 }; \
		std::abort(); }
#else
#define UTIL_DEBUG_ASSERT(operation)

#define UTIL_ASSERT(operation, what) \
	if (!(operation)) { \
		UTIL_XLOG(L"[ASSERTATION FAULT] what: "##what); \
		std::abort(); }
#endif

#else
#define UTIL_ASSERT(operation, what)
#define UTIL_ASSERT_MSGBOX(operation, what) 
#endif // __UTIL_DISABLE_ASSERT
