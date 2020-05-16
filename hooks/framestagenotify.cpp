#include "framestagenotify.hpp"

#include "../sourcesdk/hoststate.hpp"

#include "../features/playerlist.hpp"
#include "../features/esp.hpp"
#include "../features/customdisconnect.hpp"
#include "../features/textradar.hpp"
#include "../features/triggerbot.hpp"
#include "../features/namechanger.hpp"
#include "../features/spectatorlist.hpp"
#include "../features/chatspam.hpp"
#include "../gmod/luasdk/luainterface.hpp"

#include "../shutdown.hpp"

void __stdcall Hooks::FrameStageNotify(SourceSDK::ClientFrameStage stage)
{
	_SHUTDOWN_GUARD;

	switch (stage)
	{
		case SourceSDK::ClientFrameStage::Undefined:
			break;

		case SourceSDK::ClientFrameStage::Start:
			#if BUILD_GAME_IS_GMOD
			GarrysMod::LuaInterface::TryToInitialize();
			#endif
			Features::NameChanger::Update();
			Features::ChatSpam::Think();
			Features::CustomDisconnect::Think();
			break;

		case SourceSDK::ClientFrameStage::NetUpdateStart:
			break;

		case SourceSDK::ClientFrameStage::NetUpdatePostDataUpdateStart:
			break;

		case SourceSDK::ClientFrameStage::NetUpdatePostDataUpdateEnd:
			break;

		case SourceSDK::ClientFrameStage::NetUpdateEnd:
			Features::Esp::Update();
			Features::PlayerList::Update();
			Features::TextRadar::Update();
			Features::SpectatorList::Update();
			break;

		case SourceSDK::ClientFrameStage::RenderStart:
			break;

		case SourceSDK::ClientFrameStage::RenderEnd:
			break;

		default:
			break;
	}

	reinterpret_cast<decltype(Hooks::FrameStageNotify)*>
		(hooks->oldFrameStageNotify->GetOriginal())
		(stage);
}
