#pragma once
#include "assert.hpp"

#ifdef __UTIL_DISABLE_LOGS
#define __UTIL_DISABLE_WINAPI_ASSERT
#endif

#ifndef __UTIL_DISABLE_WINAPI_ASSERT
namespace Util
{
	namespace Debug
	{
		extern void MessageBoxAssert(const wchar_t* what) noexcept;
		extern void LogLastWinapiError() noexcept;
	}
}

#define UTIL_ASSERT_MSGBOX(operation, what) \
	if (!(operation)) \
		Util::Debug::MessageBoxAssert(UTIL_CXOR(what))

#ifdef _DEBUG
#define UTIL_DEBUG_ASSERT_MSGBOX(operation) \
	UTIL_ASSERT_MSGBOX(operation, L#operation)
#else
#define UTIL_DEBUG_ASSERT_MSGBOX(operation)
#endif

#define UTIL_WINAPI_ASSERT(operation, what) \
	if (!(operation)) { \
		Util::Debug::LogLastWinapiError(); \
		UTIL_XLOG(L"[WINAPI ASSERTATION FAULT] what: "##what); \
		std::abort(); }

#else
#define UTIL_ASSERT_MSGBOX(operation, what)
#define UTIL_DEBUG_ASSERT_MSGBOX(operation)
#endif // __UTIL_DISABLE_WINAPI_ASSERT
