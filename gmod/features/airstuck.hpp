#pragma once
#include <cstdint>

namespace GarrysMod
{
	namespace Features
	{
		namespace AirStuck
		{
			void Think(std::uintptr_t clmove_frame, bool& sendPacket) noexcept;
		}
	}
}
