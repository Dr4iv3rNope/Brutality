#include "createmove.hpp"

#include "../features/bhop.hpp"
#include "../features/fastwalk.hpp"

#include "../sourcesdk/hlinput.hpp"
#include "../sourcesdk/usercmd.hpp"

#include "../util/pattern.hpp"
#include "../util/debug/assert.hpp"

#include "../shutdown.hpp"

using namespace SourceSDK;

bool __fastcall Hooks::CreateMove(SourceSDK::ClientModeShared* clientMode, void* edx, float input_sample_frametime, SourceSDK::UserCmd* cmd) noexcept
{
	SHUTDOWN_HOOK_GUARD(L"CreateMove");
	MAKE_BUSY_SHUTDOWN_GUARD;

	reinterpret_cast<decltype(Hooks::CreateMove)*>
		(hooks->oldCreateMove->GetOriginal())
		(clientMode, edx, input_sample_frametime, cmd);

	Features::BunnyHop::Think(cmd);
	Features::FastWalk::Think(cmd);
	return false;
}
