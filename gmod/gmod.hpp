#pragma once
#include "../build.hpp"

#if BUILD_GAME_IS_GMOD
struct IDirect3DDevice9;

namespace SourceSDK
{
	class HLClient;
	class ClientEntityList;
	class ViewRender;
	class ClientModeShared;
	class Input;
	struct GlobalVars;
	class EngineTraceClient;
	class EngineClient;
	class GameEventManager;
	class Render;
	class ClientState;
	class MatSystemSurface;
	class MaterialSystem;
	class LocalizedStringTable;
	//class Panel;
	class CVar;
	class InputSystem;
	class ModelRender;
	class MDLCache;
	class RenderView;
}

namespace GarrysMod
{
	class LuaInterface;
}

class GmodInterfaces final
{
public:
	IDirect3DDevice9* device;
	SourceSDK::HLClient* clientDLL;
	SourceSDK::ClientEntityList* entitylist;
	SourceSDK::ViewRender* viewrender;
	SourceSDK::ClientModeShared* clientmode;
	SourceSDK::Input* input;
	SourceSDK::GlobalVars* globals;
	SourceSDK::EngineTraceClient* enginetrace;
	SourceSDK::EngineClient* engine;
	SourceSDK::GameEventManager* eventmanager;
	SourceSDK::Render* render;
	SourceSDK::ClientState* clientstate;
	SourceSDK::MatSystemSurface* surface;
	SourceSDK::MaterialSystem* materialsystem;
	SourceSDK::LocalizedStringTable* localize;
	//SourceSDK::Panel* panel;
	SourceSDK::CVar* cvar;
	SourceSDK::InputSystem* inputsystem;
	SourceSDK::ModelRender* modelrender;
	SourceSDK::MDLCache* modelcache;
	SourceSDK::RenderView* renderview;
	GarrysMod::LuaInterface** luainterface;

	GmodInterfaces();
};
using GameInterfaces = GmodInterfaces;


namespace Util
{
	namespace Vmt
	{
		class HookedMethod;
	}
}

class GmodHooks final
{
public:
	Util::Vmt::HookedMethod* oldCreateMove;
	Util::Vmt::HookedMethod* oldEndScene;
	Util::Vmt::HookedMethod* oldFrameStageNotify;
	Util::Vmt::HookedMethod* oldDrawModelExecute;
	Util::Vmt::HookedMethod* oldRenderView;
	Util::Vmt::HookedMethod* oldOverrideView;
	Util::Vmt::HookedMethod* oldReset;

	GmodHooks();

	void Initialize() noexcept;

	~GmodHooks();
};
using GameHooks = GmodHooks;
#endif
