#pragma once
//#define __UTIL_DISABLE_LOGS

#ifndef __UTIL_DISABLE_LOGS
#include <string>

#include "../xorstr.hpp"

#define __UTIL_LOG_CALLS
#ifdef _DEBUG
#define UTIL_LOG(text) Util::Debug::LogSystem::Log((text), UTIL_SXOR(__FILEW__), __LINE__)	// Logs info to file
#else
#define UTIL_LOG(text) Util::Debug::LogSystem::Log(text)									// Logs info to file
#endif // !_DEBUG
#define UTIL_XLOG(text) UTIL_LOG(UTIL_XOR(text))						// Logs xor'd info to file
#define UTIL_OPLOG(operation) { UTIL_XLOG(L#operation); operation; }	// Logs operation to file

#ifdef _DEBUG
#define UTIL_DEBUG_LOG(text) UTIL_LOG(text)
#define UTIL_DEBUG_XLOG(text) UTIL_XLOG(text)
#define UTIL_DEBUG_OPLOG(operation) UTIL_OPLOG(operation)
#else
#define UTIL_DEBUG_LOG(text)
#define UTIL_DEBUG_XLOG(text)
#define UTIL_DEBUG_OPLOG(operation)
#endif

namespace Util
{
	namespace Debug
	{
		namespace LogSystem
		{
			extern bool IsInitialized();
			extern const std::wstring& GetLogFilePath();
			extern void Initialize(const std::wstring& path);
			extern void Shutdown();

			#ifdef _DEBUG
			void Log(const std::wstring& text, const std::wstring& file, int line);
			#else
			void Log(const std::wstring& text);
			#endif
		}
	}
}
#else
#define UTIL_LOG(text)
#define UTIL_XLOG(text)
#define UTIL_OPLOG(operation)
#endif // __UTIL_DISABLE_LOGS
