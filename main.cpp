#define NOMINMAX
#include "main.hpp"

#include "util/debug/logs.hpp"
#include "util/debug/winapi.hpp"
#include "util/debug/errors.hpp"

#include "config/variable.hpp"

#include <filesystem>

#include <Windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

#undef CreateEvent

#define __INLINE_RUN_FUNC(name) \
	class _##name final { \
		public: inline _##name() noexcept; \
	} __irf##name; inline _##name::_##name() noexcept
	

//
// implementing local path system
//

static std::wstring localPath;

bool Main::IsLocalPathInitialized()
{
	return !localPath.empty();
}

const std::wstring& Main::GetLocalPath()
{
	if (!IsLocalPathInitialized())
	{
		wchar_t* folderPath;

		if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DONT_VERIFY, nullptr, &folderPath) != S_OK)
		{
			// prevent infinity loop/stack overflow
			UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get documents folder");

			UTIL_RUNTIME_ERROR("failed to get documents folder");
		}

		localPath = std::wstring(folderPath);
		localPath += UTIL_SXOR(L"\\Brutality\\");

		if (!std::filesystem::exists(localPath))
		{
			if (!std::filesystem::create_directory(localPath))
			{
				// prevent infinity loop/stack overflow
				UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get documents folder");

				UTIL_RUNTIME_ERROR("failed to create hack folder");
			}
		}

		CoTaskMemFree(folderPath);
	}

	return localPath;
}

//
// initializing logs
//

namespace
{
	__INLINE_RUN_FUNC(InitLogs)
	{
		Util::Debug::LogSystem::Initialize(Util::Debug::LogSystem::GetLogFilePath());
	}
}


//
// initializing interfaces
//

#include "sourcesdk/interfaces.hpp"

#include "sourcesdk/cvar.hpp"
#include "sourcesdk/inputsystem.hpp"
#include "sourcesdk/hlclient.hpp"
#include "sourcesdk/engineclient.hpp"
#include "sourcesdk/enginetraceclient.hpp"
#include "sourcesdk/entitylist.hpp"
#include "sourcesdk/gameevents.hpp"
#include "sourcesdk/modelcache.hpp"
#include "sourcesdk/surface.hpp"
#include "sourcesdk/localize.hpp"

#include "sourcesdk/viewrender.hpp"
#include "sourcesdk/render.hpp"
#include "sourcesdk/clientmode.hpp"
#include "sourcesdk/hlinput.hpp"
#include "sourcesdk/hoststate.hpp"
#include "sourcesdk/clientstate.hpp"
#include "sourcesdk/globals.hpp"
#include "sourcesdk/shaderapi.hpp"

#if SOURCE_SDK_IS_GMOD
SOURCE_SDK_INTERFACE(SourceSDK::CVar, SourceSDK::cvar, "vstdlib.dll", "VEngineCvar007");
SOURCE_SDK_INTERFACE(SourceSDK::CInputSystem, SourceSDK::inputsystem, "inputsystem.dll", "InputSystemVersion001");
SOURCE_SDK_INTERFACE(SourceSDK::HLClient, SourceSDK::clientDLL, "client.dll", "VClient017");
SOURCE_SDK_INTERFACE(SourceSDK::EngineTraceClient, SourceSDK::enginetrace, "engine.dll", "EngineTraceClient003");
SOURCE_SDK_INTERFACE(SourceSDK::EngineClient, SourceSDK::engine, "engine.dll", "VEngineClient013");
SOURCE_SDK_INTERFACE(SourceSDK::ClientEntityList, SourceSDK::entitylist, "client.dll", "VClientEntityList003");
SOURCE_SDK_INTERFACE(SourceSDK::IGameEventManager, SourceSDK::gameevents, "engine.dll", "GAMEEVENTSMANAGER002");
SOURCE_SDK_INTERFACE(SourceSDK::CMDLCache, SourceSDK::mdlCache, "datacache.dll", "MDLCache004");
SOURCE_SDK_INTERFACE(SourceSDK::MatSystemSurface, SourceSDK::surface, "vguimatsurface.dll", "VGUI_Surface030");
SOURCE_SDK_INTERFACE(SourceSDK::LocalizedStringTable, SourceSDK::localize, "vgui2.dll", "VguiLocalize004");
//SOURCE_SDK_INTERFACE(SourceSDK::Panel, SourceSDK::panel, "vgui2.dll", "VGUI_Panel009");

// "freezecam_started"
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::ViewRender,
	SourceSDK::view,
	"client.dll",
	"8B 0D ?? ?? ?? ?? F3 0F 10 45 ?? 51 F3 0F 11 04 ?? 8B 01 FF 90 ?? ?? ?? ?? 5F 5E",
	2, 2
);

/* "Displacement_Rendering"

mov		ecx, g_pRender
push		[ebp + arg_C]
push		[ebp + arg_8]
mov		eax, [ecx]
call		dword ptr[eax + 2Ch]
movzx	eax, byte ptr[eax + 24h]
push		eax
mov		eax, [ebp + var_14]
push		eax
push		ebx
push		[ebp + arg_0]
call		sub_100D9960
mov		edx, ds:g_VProfCurrentProfile
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::Render,
	SourceSDK::render,
	"engine.dll",
	"8B 0D ?? ?? ?? ?? FF 75 ?? FF 75 ?? 8B 01 FF 50 ?? 0F B6 40", // uid: 9360
	2, 2
);

/*
call    GetClientModeNormal ; #STR: "CBaseViewport", "CHudViewport"
mov     g_pClientMode, eax
call    sub_101F6970
push    offset aScriptsVguiScr ; "scripts/vgui_screens.txt"
mov     ecx, eax
mov     edx, [eax]
call    dword ptr [edx]
retn
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::ClientModeShared,
	SourceSDK::clientMode,
	"client.dll",
	"E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B C8 8B 10",
	6, 2
);

/* "playername"

push    offset state_m_vecEyeAngles
push    offset state_m_vecEyePosition
mov     ecx, esi
fstp    [ebp+flZFar]
call    dword ptr [eax+468h]
mov     ecx, engine
mov     eax, [ecx]
mov     eax, [eax+150h]
call    eax
test    al, al
jnz     loc_101471EC
mov     ecx, input
mov     eax, [ecx]
mov     eax, [eax+7Ch]
call    eax
test    eax, eax
jz      loc_101471EC
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::Input,
	SourceSDK::input,
	"client.dll",
	// uid: 11465127236496068054
	"8B 0D ?? ?? ?? ?? 8B 01 8B 40 ?? FF D0 85 C0 0F 84 ?? ?? ?? ?? A1",
	2, 2
);

/* "Save file %s can't be found!\n"

push    100h
push    esi; char*
push    offset byte_1069D0F0; char*
call    Q_strncpy
mov     ecx, dword_109CAE40
add     esp, 0Ch
cmp     dword_1064BA3C, 2
setnl   al
mov     edx, [ecx]
movzx   eax, al
push    eax
push    esi
call    dword ptr[edx + 84h]
mov     al, [ebp + remember_location]
mov     byte_1069D1F5, al
mov     word_1069D1F6, 100h
test    al, al
jz      short loc_101A343E
mov     ecx, offset g_HostState
call    RememberLocation
mov     g_HostState_NextState, 1
pop     esi
pop     ebp
retn
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::HostState,
	SourceSDK::nextHostState,
	"engine.dll",
	"C7 05 ?? ?? ?? ?? 01 00 00 00 5E 5D C3", // uid: 1692443618314294131
	2, 1
);

/*
push    offset aGuid    ; "guid"
call    sub_10257D70
mov     ecx, offset g_clientState
call    sub_100BD260
push    ecx
fstp    [esp+0Ch+var_C] ; int
push    offset aTime    ; "time"
mov     ecx, esi
call    sub_10257D40
xor     eax, eax
mov     ecx, esi
cmp     [edi], al
setnz   al
push    eax             ; int
push    offset aStaticsound ; "staticsound"
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::ClientState,
	SourceSDK::clientState,
	"engine.dll",
	"B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 51 D9 1C 24 68", // uid: 12854204129183460047
	1, 1
);

/*
mov     eax, gpGlobals
shl     esi, 8
add     esi, [ebx+218h]
movss   xmm0, dword ptr [eax+0Ch]
addss   xmm0, ds:dword_104D3E00
mov     eax, [edi]
lea     ecx, [esi+100h]
push    ecx
push    offset aHeading ; "heading"
mov     ecx, edi
movss   dword ptr [ebx+204h], xmm0
call    dword ptr [eax+3F0h]
mov     eax, [edi]
lea     ecx, [esi+2FEh]
push    ecx
mov     [ebp+var_4], ecx
mov     ecx, edi
push    offset aTitle   ; "title"
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	SourceSDK::GlobalVars,
	SourceSDK::globals,
	"client.dll",
	"A1 ?? ?? ?? ?? C1 E6 ?? 03 B3", // uid: 6779721209880579392
	1, 2
);

/* "mat_queue_mode", "-nulldevice"

push    5E740DE1h
mov     ecx, edi
mov     g_pDevice, esi
call    sub_1004E000
and     byte ptr [edi+0A8h], 0FDh
mov     esi, [ebp+arg_8]
mov     edx, [ebp+arg_0]
and     byte ptr [edi+10h], 0FDh
mov     cl, [edi+0A8h]
mov     [edi+8], edx
mov     [edi+24h], ebx
mov     [edi+4], ebx
mov     dword ptr [edi+0A4h], 0
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	IDirect3DDevice9,
	SourceSDK::device,
	"shaderapidx9.dll",
	"8B 35 ?? ?? ?? ?? 8B CB", // uid: 2736
	2, 2
);
#endif

//
// initializing helpers
//

#include "imgui/custom/windowmanager.hpp"

#include "sourcesdk/netvars.hpp"
#include "sourcesdk/clientclassdumper.hpp"

SourceSDK::NetvarManager SourceSDK::netvars;
SourceSDK::ClientClassDumper SourceSDK::clientClassDumper;

ImGui::Custom::WindowManager ImGui::Custom::windowManager;

//
// initializing garrysmod interfaces
//
#if SOURCE_SDK_IS_GMOD
#include "features/gmod/luainterface.hpp"
#include "features/gmod/antiscreengrab.hpp"

/*
mov     ecx, luaInterface
lea     edx, [ebp+lua_code_str]
push    1
push    1
mov     [ebp+var_4], 0
mov     eax, [ecx]
push    edx
push    offset empty_string
push    offset aLuacmd  ; "LuaCmd"
*/
SOURCE_SDK_INTERFACE_PATTERN
(
	Features::GarrysMod::LuaInterface*,
	Features::GarrysMod::luaInterface,
	"client.dll",
	"8B 0D ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? 6A 01 6A 01",
	2, 1
);

//
// anti-screengrab
//

Config::Bool antiScreenGrabEnable(true, UTIL_SXOR("Protection"), UTIL_SXOR("Anti-Screen Grab"), false);

namespace
{
	__INLINE_RUN_FUNC(InitScreenGrab)
	{
		Features::GarrysMod::AntiScreenGrab::Initialize();
	}
}

//
// run string
//

Config::Bool preventRunString(true, UTIL_SXOR("Protection"), UTIL_SXOR("Prevent Run String"), false);

#endif

//
// initialize hooks
//
#include "hooks/createmove.hpp"
#include "hooks/endscene.hpp"
#include "hooks/framestagenotify.hpp"
#include "hooks/overrideview.hpp"
#include "hooks/reset.hpp"
#include "hooks/renderview.hpp"

Util::Vmt::HookedMethod* Hooks::oldCreateMove
{
	new Util::Vmt::HookedMethod(SourceSDK::clientMode, SourceSDK::clientMode->GetCreateMoveIndex())
};

Util::Vmt::HookedMethod* Hooks::oldEndScene
{
	new Util::Vmt::HookedMethod(SourceSDK::device, 42)
};

Util::Vmt::HookedMethod* Hooks::oldFrameStageNotify
{
	new Util::Vmt::HookedMethod
	(
		SourceSDK::clientDLL,
		SourceSDK::clientDLL->GetFrameStageNotifyIndex()
	)
};

Util::Vmt::HookedMethod* Hooks::oldOverrideView
{
	new Util::Vmt::HookedMethod
	(
		SourceSDK::clientMode,
		SourceSDK::clientMode->GetOverrideViewIdx()
	)
};

Util::Vmt::HookedMethod* Hooks::oldReset
{
	new Util::Vmt::HookedMethod(SourceSDK::device, 16)
};

/*Util::Vmt::HookedMethod* Hooks::oldPaintTraverse
{
	new Util::Vmt::HookedMethod(SourceSDK::panel, SourceSDK::panel->GetPaintTraverseIndex())
};*/

Util::Vmt::HookedMethod* Hooks::oldRenderView
{
	new Util::Vmt::HookedMethod(SourceSDK::view, SourceSDK::view->GetRenderViewIndex())
};

namespace
{
	__INLINE_RUN_FUNC(InitHooks)
	{
		Hooks::oldCreateMove->Initialize(Hooks::CreateMove);
		Hooks::oldEndScene->Initialize(Hooks::EndScene);
		Hooks::oldFrameStageNotify->Initialize(Hooks::FrameStageNotify);
		Hooks::oldOverrideView->Initialize(Hooks::OverrideView);
		Hooks::oldReset->Initialize(Hooks::Reset);
		Hooks::oldRenderView->Initialize(Hooks::RenderView);
		//Hooks::oldPaintTraverse->Initialize(Hooks::PaintTraverse);
	}
}

//
// initializing config variables
//

#ifdef _DEBUG
// debug variables

static Config::Bool v_bool(true, "Debug", "Boolean");
static Config::Int32 v_int32(true, "Debug", "Integer 32");
static Config::UInt32 v_uint32(true, "Debug", "Unsigned Integer 32");
static Config::Float v_float(true, "Debug", "Float");

static Config::LInt32 lv_int32(true, "Debug", "Limited Integer 32", 0, -228, 1337);
static Config::LUInt32 lv_uint32(true, "Debug", "Limited Unsigned Integer 32", 666, 666, 1234);
static Config::LFloat lv_float(true, "Debug", "Limited Float", 0, -3.14f, 2.28f);
static Config::LimitedString<char> lv_string(true, "Debug", "Your Name", "AYE", 64);

static Config::String<char> uv_string(true, "Debug", "XYZ", "123");
static Config::Enum uv_enum(true, "Debug", "Who are you", { "Idiot", "I Have Stupid", "Yes sir" });
static Config::Color uv_color(true, "Debug", "Pick some color", 0x13372280);
static Config::Key uv_key(true, "Debug", "Some Key Binding");
#endif

Config::Color playerColorNormal(true, UTIL_SXOR("Player Colors"), UTIL_SXOR("Normals Color"), { 255, 255, 255, 255 });
Config::Color playerColorDangerous(true, UTIL_SXOR("Player Colors"), UTIL_SXOR("Dangerouses Color"), { 255, 0, 0, 155 });
Config::Color playerColorFriends(true, UTIL_SXOR("Player Colors"), UTIL_SXOR("Friends Color"), { 55, 255, 55, 255 });
Config::Color playerColorRages(true, UTIL_SXOR("Player Colors"), UTIL_SXOR("Rages Color"), { 255, 100, 0, 255 });


// esp

Config::Bool espEnabled(true, UTIL_SXOR("ESP"), UTIL_SXOR("Enable"), false);
Config::LFloat espMaxDistance(true, UTIL_SXOR("ESP"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool espUpdatePerFrame(true, UTIL_SXOR("ESP"), UTIL_SXOR("Update Per Frame"), false);

Config::Bool espDrawEntities(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Entities"), false);
Config::Bool espDrawNormal(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Normal"), false);
Config::Bool espDrawDangerous(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dangerous"), false);
Config::Bool espDrawFriends(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Friends"), false);
Config::Bool espDrawRages(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Rages"), false);
Config::Bool espDrawDeadPlayers(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dead Players"), false);
Config::Bool espDrawDormantPlayers(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dormant Players"), false);

Config::Bool espHealth(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Health"), false);
Config::Color espHealthColor(true, UTIL_SXOR("ESP"), UTIL_SXOR("Health Text Color"), { 55, 255, 55, 255 });

Config::Bool espName(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Name"), false);
Config::Color espNameColor(true, UTIL_SXOR("ESP"), UTIL_SXOR("Name Text Color"), { 55, 255, 255, 255 });

Config::Bool espSkeleton(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Skeleton"), false);
#ifdef _DEBUG
Config::Bool dbg_espSkeleton(true, UTIL_SXOR("ESP"), UTIL_SXOR("Debug Skeleton"), false);
#endif
Config::LFloat espSkeletonThickness(true, UTIL_SXOR("ESP"), UTIL_SXOR("Skeleton Thickness"), 1.f, 0.1f, 12.f);

Config::Bool espActiveWeapon(true, UTIL_SXOR("ESP"), UTIL_SXOR("Draw Active Weapon"), false);
Config::Color espActiveWeaponColor(true, UTIL_SXOR("ESP"), UTIL_SXOR("Active Weapon Text Color"), { 55, 55, 55, 255 });


// text radar

Config::Bool textRadarEnable(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("Enable"), false);

Config::UInt32 textRadarX(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("X"), 0);
Config::UInt32 textRadarY(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("Y"), 0);

Config::LUInt32 textRadarMaxPlayers(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Players"), 10, 5, 32);
Config::LFloat textRadarMaxDistance(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool textRadarDrawDistance(true, UTIL_SXOR("Text Radar"), UTIL_SXOR("Draw Distance"), false);


// crosshair

Config::Bool crosshairEnable(true, UTIL_SXOR("Crosshair"), UTIL_SXOR("Enable"), false);

Config::LUInt32 crosshairGap(true, UTIL_SXOR("Crosshair"), UTIL_SXOR("Gap"), 0, 0, 50);
Config::LUInt32 crosshairSize(true, UTIL_SXOR("Crosshair"), UTIL_SXOR("Size"), 1, 1, 50);
Config::LUInt32 crosshairThickness(true, UTIL_SXOR("Crosshair"), UTIL_SXOR("Thickness"), 1, 1, 10);

Config::Color crosshairColor(true, UTIL_SXOR("Crosshair"), UTIL_SXOR("Color"), { 255, 255, 255, 255 });


// bunny hop

Config::Bool bhopEnable(true, UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Enable"), false);
Config::Bool bhopAutoStrafe(true, UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Mouse Auto-Strafe"), false);

/*
Config::LFloat bhopMinDelay(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Min Jump Delay"), 0.f, 0.f, 1000.f);
Config::LFloat bhopMaxDelay(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Max Jump Delay"), 0.f, 0.f, 1000.f);
*/


// user interface settings

Config::Bool useTabs(false, UTIL_SXOR("Interface"), UTIL_SXOR("Use Tabs instead of windows"), false);


//
// name changer
//
#include "features/namechanger.hpp"

Config::String<char> nameChangerName(false, UTIL_SXOR("Name Changer"), UTIL_SXOR("Name"), std::string());
Config::Bool nameChangerAutoSend(false, UTIL_SXOR("Name Changer"), UTIL_SXOR("Auto Send"), false);

namespace
{
	__INLINE_RUN_FUNC(InitNameChanger)
	{
		Features::NameChanger::Initialize();
	}
}


//
// Registering menu windows
//
#include "features/chatspam.hpp"
#include "features/playerlist.hpp"
#include "features/customdisconnect.hpp"
#include "features/esp.hpp"
#include "features/gmod/lualoader.hpp"

#include "config/config.hpp"

namespace
{
	__INLINE_RUN_FUNC(RegisterWindows)
	{
		Features::NameChanger::RegisterWindow();
		Features::ChatSpam::RegisterWindow();
		Features::Esp::RegisterEntityListWindow();
		Features::PlayerList::RegisterWindow();
		Features::CustomDisconnect::RegisterWindow();
		#if SOURCE_SDK_IS_GMOD
		Features::GarrysMod::LuaLoader::RegisterWindow();
		#endif

		Config::RegisterVariablesInWindowManager();
		Config::RegisterWindow();
	}
}
