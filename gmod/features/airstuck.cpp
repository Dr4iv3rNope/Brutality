#include "airstuck.hpp"
#if BUILD_GAME_IS_GMOD

#include "../../sourcesdk/hlclient.hpp"

#include "../../main.hpp"

#include "../../config/variable.hpp"

extern Config::Bool airstuckEnable;
extern Config::Key airstuckKey;
extern Config::LUInt32 airstuckMaxTicks;

void GarrysMod::Features::AirStuck::Think(std::uintptr_t clmove_frame, bool& sendPacket) noexcept
{
	static std::uint32_t ticksStucked = 0;

	if (airstuckEnable &&
		airstuckKey.HasValue() &&
		ImGui::Custom::GetAsyncKeyState(airstuckKey.GetValue()) &&
		ticksStucked <= *airstuckMaxTicks)
	{
		sendPacket = true;

		ticksStucked++;
		*reinterpret_cast<std::uint32_t*>(clmove_frame + 4) -= 5;
	}
	else
		ticksStucked = 0;
}

#endif
