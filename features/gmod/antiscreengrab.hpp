#pragma once
#include "../../sourcesdk/sdk.hpp"

#if SOURCE_SDK_IS_GMOD
namespace Features
{
	namespace GarrysMod
	{
		namespace AntiScreenGrab
		{
			extern void Initialize();
			extern void Shutdown();
		}
	}
}
#endif
