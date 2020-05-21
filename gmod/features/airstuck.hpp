#pragma once
#include "../../build.hpp"
#if BUILD_GAME_IS_GMOD

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

#endif
