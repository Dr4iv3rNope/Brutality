#include "createmove.hpp"

#include "../features/bhop.hpp"
#include "../features/fastwalk.hpp"
#include "../features/triggerbot.hpp"
#include "../gmod/features/airstuck.hpp"

#include "../sourcesdk/hlinput.hpp"
#include "../sourcesdk/usercmd.hpp"

#include "../util/pattern.hpp"
#include "../util/debug/assert.hpp"

#include "../shutdown.hpp"

using namespace SourceSDK;

bool __fastcall Hooks::CreateMove(SourceSDK::ClientModeShared* clientMode, void* edx, float input_sample_frametime, SourceSDK::UserCmd* cmd) noexcept
{
	_SHUTDOWN_GUARD;

	bool* sendPacket = nullptr;
	std::uintptr_t clmove_framePointer = 0;

	{
		std::uintptr_t*** framePointer;

		__asm mov framePointer, ebp;

		clmove_framePointer = ***framePointer;
		sendPacket = reinterpret_cast<bool*>(clmove_framePointer - 1);
	}

	UTIL_DEBUG_ASSERT(sendPacket);
	UTIL_DEBUG_ASSERT(clmove_framePointer);

	reinterpret_cast<decltype(Hooks::CreateMove)*>
		(hooks->oldCreateMove->GetOriginal())
		(clientMode, edx, input_sample_frametime, cmd);

	Features::BunnyHop::Think(cmd);
	Features::FastWalk::Think(cmd);
	Features::TriggerBot::Think(cmd);
	#if BUILD_GAME_IS_GMOD
	GarrysMod::Features::AirStuck::Think(clmove_framePointer, *sendPacket);
	#endif

	return false;
}
