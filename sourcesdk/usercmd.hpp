#pragma once
#include "../build.hpp"

#include "convar.hpp"
#include "cvar.hpp"
#include "vector.hpp"
#include "inputbuttons.hpp"

namespace SourceSDK
{
	extern float GetMaxForwardSpeed() noexcept;
	extern float GetMaxSideSpeed() noexcept;
	extern float GetMaxUpSpeed() noexcept;

	struct UserCmd final
	{
		int commandNumber;
		int tickCount;
		Angle viewAngles;
		Vector3 move;
		InButtons buttons;
		std::uint8_t impulse;
		int weaponSelect;
		int weaponSubtype;
		int randomSeed;
		short mouseX;
		short mouseY;
		bool hasBeenPredicted;

		inline void SetButton(Input_ button) noexcept { this->buttons.SetFlag(button); }
		inline void RemoveButton(Input_ button) noexcept { this->buttons.RemoveFlag(button); }
		inline bool HasButton(Input_ button) const noexcept { return this->buttons.HasFlag(button); }
	};
}
