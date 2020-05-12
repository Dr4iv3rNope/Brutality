#include "inputsystem.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"

void SourceSDK::InputSystem::EnableInput(bool enable)
{
	// "Unable to create game window" in CGame::CreateGameWindow
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"FF 50 ?? 8B 0D ?? ?? ?? ?? 6A 01 8B 01 FF 50 ?? 8B 0D",
			15
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, bool>(this, offset, enable);
}

bool SourceSDK::InputSystem::IsButtonDown(ButtonCode code)
{
	// "scores" in CClientScoreBoardDialog::OnThink
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"52 8B 01 8B 40 ?? FF D0 84 C0 75 ?? C7 86 ?? ?? ?? ?? FF FF FF FF",
			5
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<bool, ButtonCode>(this, offset, code);
}

void SourceSDK::InputSystem::ResetInputState()
{
	// in CInputSystem::DetachFromWindow
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B 06 FF 90 ?? ?? ?? ?? 8B 86 ?? ?? ?? ?? 85 C0",
			4
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void>(this, offset);
}
