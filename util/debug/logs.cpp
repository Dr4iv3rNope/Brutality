#include "logs.hpp"

#ifndef __UTIL_DISABLE_LOGS
#include "errors.hpp"
#include "winapi.hpp"

#include "../strings.hpp"
#include "../../main.hpp"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <functional>
#include <mutex>

static std::wofstream* logStream{ nullptr };

inline auto& GetLogStreamMutex() noexcept
{
	static std::mutex logStreamMutex {};

	return logStreamMutex;
}

bool Util::Debug::LogSystem::IsInitialized()
{
	return logStream != nullptr;
}

const std::wstring& Util::Debug::LogSystem::GetLogFilePath()
{
	static std::wstring logFilePath
	{
		UTIL_WFORMAT(Main::GetLocalPath() << UTIL_XOR(L"logs.txt"))
	};

	return logFilePath;
}

void Util::Debug::LogSystem::Initialize(const std::wstring& path)
{
	GetLogStreamMutex().lock();
	UTIL_DEBUG_ASSERT(!logStream);

	logStream = new std::wofstream(path);

	UTIL_ASSERT_MSGBOX(logStream, L"Failed to allocate log stream");
	UTIL_ASSERT_MSGBOX(logStream->good(), L"Failed to create log stream");
	GetLogStreamMutex().unlock();

	UTIL_XLOG(L"Log Stream Opened");
}

void Util::Debug::LogSystem::Shutdown()
{
	UTIL_DEBUG_ASSERT_MSGBOX(logStream);

	UTIL_XLOG(L"Log Stream Closed\n\n");

	logStream->close();
	delete logStream;
}

void LogFull(std::function<void(std::wstringstream&)> callback)
{
	UTIL_DEBUG_ASSERT_MSGBOX(logStream);

	std::wstringstream formated;

	if (callback)
		callback(formated);

	GetLogStreamMutex().lock();
	(*logStream) << formated.str() << std::endl;
	GetLogStreamMutex().unlock();
}

void LogTime(std::wstringstream& formated)
{
	auto time = std::time(nullptr);

	tm localtime;
	if (!::localtime_s(&localtime, &time))
		formated << std::put_time(&localtime, UTIL_CXOR(L"[%d-%m-%Y %H:%M:%S]"));
	else
		formated << UTIL_SXOR(L"[NO TIME]");
}

#ifdef _DEBUG
void Util::Debug::LogSystem::Log(const std::wstring& text, const std::wstring& file, int line)
{
	LogFull([&text, &file, &line] (std::wstringstream& formated)
	{
		LogTime(formated);

		formated << UTIL_SXOR(L" [") << file << L':' << line << UTIL_SXOR(L"] ") << text;
	});
}
#else
void Util::Debug::LogSystem::Log(const std::wstring& text)
{
	LogFull([&text] (std::wstringstream& formated)
	{
		LogTime(formated);

		formated << ' ' << text;
	});
}
#endif // !_DEBUG
#endif // __UTIL_DISABLE_LOGS
