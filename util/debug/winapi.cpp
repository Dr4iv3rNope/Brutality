#include "winapi.hpp"

#include "../strings.hpp"

#include <Windows.h>

void Util::Debug::MessageBoxAssert(const wchar_t* what) noexcept
{
	MessageBoxW
	(
		nullptr,
		what,
		UTIL_CXOR(L"Brutality Assertation Fault"),
		MB_OK | MB_ICONERROR
	);

	if (Util::Debug::LogSystem::IsInitialized())
		UTIL_LOG(UTIL_WFORMAT(UTIL_XOR(L"[MSG BOX ASSERTATION FAULT] ") << what));

	#ifdef _DEBUG
	__asm int 3;
	#endif

	std::abort();
}

void Util::Debug::LogLastWinapiError() noexcept
{
	const auto last_error = GetLastError();

	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"LastError: ")
		<< last_error
		<< UTIL_XOR(L" (Hex: 0x") << std::hex
		<< last_error << ')'
	));
}
