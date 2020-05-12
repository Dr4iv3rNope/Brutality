#pragma once
#include "../../build.hpp"

#if BUILD_GAME_IS_GMOD
namespace GarrysMod
{
	namespace Features
	{
		namespace LuaLoader
		{
			void RegisterWindow() noexcept;
		}
	}
}
#endif
