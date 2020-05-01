#pragma once
#include "vector.hpp"
#include "inputbuttons.hpp"

namespace SourceSDK
{
	constexpr float MAX_CMD_MOVE_SPEED { 10000.f };

	struct UserCmd final
	{
		int commandNumber;
		int tickCount;
		Angle viewAngles;
		Vector move;
		int buttons;
		char impulse;
		int weaponSelect;
		int weaponSubtype;
		int randomSeed;
		short mouseX;
		short mouseY;
		bool hasBeenPredicted;


		inline void AddButton(InButton button) noexcept { this->buttons |= int(button); }
		inline void RemoveButton(InButton button) noexcept { this->buttons &= ~int(button); }

		inline bool HasButton(InButton button) const noexcept { return this->buttons & int(button); }
	};
}
