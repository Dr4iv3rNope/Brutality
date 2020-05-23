#include "framestagenotify.hpp"

#include "../features/playerlist.hpp"
#include "../features/esp.hpp"
#include "../features/customdisconnect.hpp"
#include "../features/textradar.hpp"
#include "../features/triggerbot.hpp"
#include "../features/namechanger.hpp"
#include "../features/spectatorlist.hpp"
#include "../features/backtrack.hpp"
#include "../features/chatspam.hpp"
#include "../gmod/luasdk/luainterface.hpp"

#include "../sourcesdk/render.hpp"
#include "../sourcesdk/cvar.hpp"
#include "../sourcesdk/convar.hpp"

#include "../shutdown.hpp"
#include "../main.hpp"

#include <mutex>

static inline SourceSDK::VMatrix& LastWorldToScreenMatrix() noexcept
{
	static SourceSDK::VMatrix worldToScreenMatrix;

	return worldToScreenMatrix;
}

static inline std::mutex& GetLastWorldToScreenMutex() noexcept
{
	static std::mutex lastWorldToScreenMutex;

	return lastWorldToScreenMutex;
}

void Hooks::CopyLastWorldToScreenMatrix(SourceSDK::VMatrix& matrix) noexcept
{
	GetLastWorldToScreenMutex().lock();
	matrix = LastWorldToScreenMatrix();
	GetLastWorldToScreenMutex().unlock();
}

static inline void SetupLastWorldToScreenMatrix() noexcept
{
	static SourceSDK::IConVar* mat_queue_mode = interfaces->cvar->FindVar(UTIL_CXOR("mat_queue_mode"));
	UTIL_DEBUG_ASSERT(mat_queue_mode);

	if (mat_queue_mode->GetRaw()->intValue != 0)
	{
		GetLastWorldToScreenMutex().lock();
		LastWorldToScreenMatrix() = interfaces->render->WorldToScreenMatrix();
		GetLastWorldToScreenMutex().unlock();
	}
}

void __stdcall Hooks::FrameStageNotify(SourceSDK::ClientFrameStage stage)
{
	_SHUTDOWN_GUARD;

	switch (stage)
	{
		case SourceSDK::ClientFrameStage::Undefined:
			break;

		case SourceSDK::ClientFrameStage::Start:
			SetupLastWorldToScreenMatrix();

			#if BUILD_GAME_IS_GMOD
			GarrysMod::LuaInterface::TryToInitialize();
			#endif
			Features::NameChanger::Update();
			Features::ChatSpam::Think();
			Features::CustomDisconnect::Think();
			Features::Backtrack::Update();
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
