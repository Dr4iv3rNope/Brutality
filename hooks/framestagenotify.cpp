#include "framestagenotify.hpp"

#include "../sourcesdk/hoststate.hpp"

#include "../features/playerlist.hpp"
#include "../features/esp.hpp"
#include "../features/textradar.hpp"
#include "../features/namechanger.hpp"
#include "../features/gmod/luainterface.hpp"

#include "../shutdown.hpp"

void __stdcall Hooks::FrameStageNotify(SourceSDK::ClientFrameStage stage)
{
	CREATE_SHUTDOWN_HOOK_GUARD(L"FrameStageNotify", oldFrameStageNotify);
	MAKE_BUSY_SHUTDOWN_GUARD;

	switch (stage)
	{
		case SourceSDK::ClientFrameStage::Undefined:
			break;

		case SourceSDK::ClientFrameStage::Start:
			#if SOURCE_SDK_IS_GMOD
			Features::GarrysMod::LuaInterface::TryToInitialize();
			#endif
			Features::NameChanger::Update();
			break;

		case SourceSDK::ClientFrameStage::NetUpdateStart:
			break;

		case SourceSDK::ClientFrameStage::NetUpdatePostDataUpdateStart:
			break;

		case SourceSDK::ClientFrameStage::NetUpdatePostDataUpdateEnd:
			break;

		case SourceSDK::ClientFrameStage::NetUpdateEnd:
			Features::PlayerList::Update();
			Features::Esp::Update();
			Features::TextRadar::Update();
			break;

		case SourceSDK::ClientFrameStage::RenderStart:
			break;

		case SourceSDK::ClientFrameStage::RenderEnd:
			break;

		default:
			break;
	}

	reinterpret_cast<decltype(Hooks::FrameStageNotify)*>
		(oldFrameStageNotify->GetOriginal())
		(stage);
}
