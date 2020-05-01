#pragma once
#include "../../sourcesdk/sdk.hpp"

#if SOURCE_SDK_IS_GMOD
#include "../../sourcesdk/interfaces.hpp"

namespace Features
{
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

			// return true if runstring has been unhooked
			static bool Shutdown();

			bool RunString(const char* id, const char* code) noexcept;
		};

		// luaInterface can be null!
		SOURCE_SDK_INTERFACE_DECL(LuaInterface*, luaInterface);
	}
}
#endif
