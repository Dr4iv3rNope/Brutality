#pragma once
#include "../../build.hpp"

#if BUILD_GAME_IS_GMOD
namespace GarrysMod
{
	namespace Features
	{
		namespace AntiScreenGrab
		{
			extern void Initialize();
			extern void Shutdown();
		}
	}
}
#endif
