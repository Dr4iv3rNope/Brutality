#include "airstuck.hpp"

#include "../../sourcesdk/hlclient.hpp"

#include "../../util/vmt.hpp"
#include "../../util/debug/assert.hpp"

#include "../../main.hpp"

#include "../../config/variable.hpp"

extern Config::Bool airstuckEnable;
extern Config::Key airstuckKey;
extern Config::LUInt32 airstuckMaxTicks;

static Util::Vmt::HookedMethod* oldCreateMove;

static void __fastcall CreateMove(void* edx, void* ecx, int sequence_number, float input_sample_frametime, bool active) noexcept
{
	reinterpret_cast<decltype(CreateMove)*>
		(oldCreateMove->GetOriginal())
		(edx, ecx, sequence_number, input_sample_frametime, active);

	static std::uint32_t ticksStucked = 0;

	if (airstuckEnable &&
		airstuckKey.HasKeyValue() &&
		ImGui::Custom::GetAsyncKeyState(airstuckKey.GetKeyValue()) &&
		ticksStucked <= *airstuckMaxTicks)
	{
		std::uint32_t** stackPointer; // ebp

		__asm
		{
			mov bl, 1
			mov stackPointer, ebp
		};

		ticksStucked++;
		*(*stackPointer + 1) -= 5;
	}
	else
		ticksStucked = 0;
}

void GarrysMod::Features::AirStuck::Initialize() noexcept
{
	// already initialized
	UTIL_DEBUG_ASSERT(!oldCreateMove);

	oldCreateMove = new Util::Vmt::HookedMethod(
		interfaces->clientDLL,
		interfaces->clientDLL->GetCreateMoveIndex()
	);

	oldCreateMove->Initialize(CreateMove);
}

void GarrysMod::Features::AirStuck::Shutdown() noexcept
{
	// must be initialized
	UTIL_DEBUG_ASSERT(oldCreateMove);

	delete oldCreateMove;
}
