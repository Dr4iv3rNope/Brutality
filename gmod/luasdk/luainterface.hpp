#pragma once
#include "../../build.hpp"

#if BUILD_GAME_IS_GMOD
#include <cstddef>

namespace GarrysMod
{
	class LuaInterface final
	{
	private:
		static std::size_t GetRunStringIndex();

	public:
		// return true if runstring has been hooked
		static bool IsInitialized() noexcept;

		// call it whatever you want
		// we need to hook runstring function
		//
		// return true if hooked
		static bool TryToInitialize();

		static void Shutdown();

		bool RunString(const char* id, const char* code) noexcept;
	};
}
#endif
