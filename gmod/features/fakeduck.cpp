#include "fakeduck.hpp"
#if BUILD_GAME_IS_GMOD

#include "../../sourcesdk/usercmd.hpp"

#include "../../config/variable.hpp"

extern Config::Bool fakeduckEnable;
extern Config::Key fakeduckKey;

void GarrysMod::Features::FakeDuck::Think(SourceSDK::UserCmd* cmd, bool& sendPacket) noexcept
{
	/*static float lastDuckTime = 0.f;

	if (fakeduckEnable && fakeduckKey.HasKeyValue() && ImGui::Custom::GetAsyncKeyState(fakeduckKey.GetKeyValue()))
	{
		if (lastDuckTime)
		{
			sendPacket = true;

			cmd->SetButton(SourceSDK::Input_Duck);

			duckedTicks++;
		}
		else
		{
			sendPacket = false;

			cmd->RemoveButton(SourceSDK::Input_Duck);
			
			duckedTicks = 0;
		}
	}
	else
		duckedTicks = 0;*/
}

#endif