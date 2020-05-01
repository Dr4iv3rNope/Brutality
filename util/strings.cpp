#include "strings.hpp"
#include "xorstr.hpp"

#include "debug/errors.hpp"
#include "debug/winapi.hpp"

#include <Windows.h>

#include <memory>

std::wstring Util::ToWString(const std::string& str)
{
	auto size_required = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);

	if (!size_required)
	{
		Util::Debug::LogLastWinapiError();
		UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get required string length");

		UTIL_RUNTIME_ERROR("failed to get required string length");
	}

	std::unique_ptr<wchar_t> wstr(new wchar_t[size_required]{});

	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), wstr.get(), str.size()))
		return std::wstring(wstr.get(), size_required);
	else
	{
		Util::Debug::LogLastWinapiError();
		UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to convert multibyte to widechar");

		UTIL_RUNTIME_ERROR("failed to convert multibyte to widechar");
	}
}

std::string Util::ToString(const std::wstring& wstr)
{
	auto size_required = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);

	if (!size_required)
	{
		Util::Debug::LogLastWinapiError();
		UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get required string length");

		UTIL_RUNTIME_ERROR("failed to get required string length");
	}

	std::unique_ptr<char> str(new char[size_required]{});

	if (WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), str.get(), size_required, nullptr, nullptr))
		return std::string(str.get(), str.get() + size_required);
	else
	{
		Util::Debug::LogLastWinapiError();
		UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to convert widechar to multibyte");

		UTIL_RUNTIME_ERROR("failed to convert widechar to multibyte");
	}
}
