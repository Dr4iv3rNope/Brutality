#include "gmod.hpp"

#include "../main.hpp"

#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include "../sourcesdk/clientmode.hpp"
#include "../sourcesdk/hlclient.hpp"
#include "../sourcesdk/modelrender.hpp"
#include "../sourcesdk/viewrender.hpp"

#include "../hooks/createmove.hpp"
#include "../hooks/endscene.hpp"
#include "../hooks/framestagenotify.hpp"
#include "../hooks/overrideview.hpp"
#include "../hooks/reset.hpp"
#include "../hooks/renderview.hpp"
#include "../hooks/drawmodelexecute.hpp"


GmodInterfaces::GmodInterfaces()
{
	VALVE_SDK_INTERFACE_IMPL(clientDLL, "client.dll", "VClient017");
	VALVE_SDK_INTERFACE_IMPL(entitylist, "client.dll", "VClientEntityList003");

	// "freezecam_started"
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		viewrender,
		"client.dll",
		"8B 0D ?? ?? ?? ?? F3 0F 10 45 ?? 51 F3 0F 11 04 ?? 8B 01 FF 90 ?? ?? ?? ?? 5F 5E",
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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		clientmode,
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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		input,
		"client.dll",
		"8B 0D ?? ?? ?? ?? 8B 01 8B 40 ?? FF D0 85 C0 0F 84 ?? ?? ?? ?? A1",
		2, 2
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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		globals,
		"client.dll",
		"A1 ?? ?? ?? ?? C1 E6 ?? 03 B3",
		1, 2
	);

	VALVE_SDK_INTERFACE_IMPL(enginetrace, "engine.dll", "EngineTraceClient003");
	VALVE_SDK_INTERFACE_IMPL(engine, "engine.dll", "VEngineClient013");
	VALVE_SDK_INTERFACE_IMPL(eventmanager, "engine.dll", "GAMEEVENTSMANAGER002");
	VALVE_SDK_INTERFACE_IMPL(modelrender, "engine.dll", "VEngineModel016");
	VALVE_SDK_INTERFACE_IMPL(renderview, "engine.dll", "VEngineRenderView014");

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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		render,
		"engine.dll",
		"8B 0D ?? ?? ?? ?? FF 75 ?? FF 75 ?? 8B 01 FF 50 ?? 0F B6 40",
		2, 2
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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		clientstate,
		"engine.dll",
		"B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 51 D9 1C 24 68",
		1, 1
	);

	VALVE_SDK_INTERFACE_IMPL(surface, "vguimatsurface.dll", "VGUI_Surface030");
	VALVE_SDK_INTERFACE_IMPL(localize, "vgui2.dll", "VguiLocalize004");
	//VALVE_SDK_INTERFACE_DECL(panel, "vgui2.dll", "VGUI_Panel009");

	VALVE_SDK_INTERFACE_IMPL(cvar, "vstdlib.dll", "VEngineCvar007");

	VALVE_SDK_INTERFACE_IMPL(inputsystem, "inputsystem.dll", "InputSystemVersion001");

	VALVE_SDK_INTERFACE_IMPL(modelcache, "datacache.dll", "MDLCache004");

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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		device,
		"shaderapidx9.dll",
		"8B 35 ?? ?? ?? ?? 8B CB",
		2, 2
	);

	VALVE_SDK_INTERFACE_IMPL(materialsystem, "materialsystem.dll", "VMaterialSystem080");

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
	VALVE_SDK_INTERFACE_IMPL_PATTERN
	(
		luainterface,
		"client.dll",
		"8B 0D ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? 6A 01 6A 01",
		2, 1
	);
}

GmodHooks::GmodHooks()
{
	oldCreateMove = new Util::Vmt::HookedMethod(
		interfaces->clientmode,
		interfaces->clientmode->GetCreateMoveIndex()
	);

	oldOverrideView = new Util::Vmt::HookedMethod(
		interfaces->clientmode,
		interfaces->clientmode->GetOverrideViewIdx()
	);

	oldEndScene = new Util::Vmt::HookedMethod(
		interfaces->device, 42
	);

	oldReset = new Util::Vmt::HookedMethod(
		interfaces->device, 16
	);

	oldFrameStageNotify = new Util::Vmt::HookedMethod(
		interfaces->clientDLL,
		interfaces->clientDLL->GetFrameStageNotifyIndex()
	);

	oldRenderView = new Util::Vmt::HookedMethod(
		interfaces->viewrender,
		interfaces->viewrender->GetRenderViewIndex()
	);

	oldDrawModelExecute = new Util::Vmt::HookedMethod(
		interfaces->modelrender,
		interfaces->modelrender->GetDrawModelExecuteIndex()
	);
}

void GmodHooks::Initialize() noexcept
{
	oldCreateMove->Initialize(Hooks::CreateMove);
	oldEndScene->Initialize(Hooks::EndScene);
	oldFrameStageNotify->Initialize(Hooks::FrameStageNotify);
	oldOverrideView->Initialize(Hooks::OverrideView);
	oldReset->Initialize(Hooks::Reset);
	oldRenderView->Initialize(Hooks::RenderView);
	oldDrawModelExecute->Initialize(Hooks::DrawModelExecute);
}

GmodHooks::~GmodHooks()
{
	delete oldCreateMove;
	delete oldEndScene;
	delete oldFrameStageNotify;
	delete oldDrawModelExecute;
	delete oldRenderView;
	delete oldOverrideView;
	delete oldReset;
}
