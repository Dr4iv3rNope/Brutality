#include "../build.hpp"
#include "clientclass.hpp"
#include "clientclassdumper.hpp"

#include "../util/xorstr.hpp"

#include "../util/debug/assert.hpp"

bool SourceSDK::ClientClass::IsPlayer() const
{
	#if BUILD_GAME_IS_GMOD
	static auto playerClass = clientClassDumper->FindClientClass(UTIL_XOR("CGMOD_Player"));

	UTIL_ASSERT(playerClass, "Failed to find Player ClientClass");
	#endif

	return this == playerClass;
}
