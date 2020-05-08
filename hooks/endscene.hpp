#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include <d3d9.h>

namespace Hooks
{
	VALVE_SDK_INTERFACE_DECL(Util::Vmt::HookedMethod, oldEndScene);
	extern HRESULT __stdcall EndScene(IDirect3DDevice9*);
}
