#pragma once
#include "../../sourcesdk/sdk.hpp"

#if SOURCE_SDK_IS_GMOD
namespace Features
{
	namespace GarrysMod
	{
		namespace LuaLoader
		{
			void RegisterWindow() noexcept;
		}
	}
}
#endif
