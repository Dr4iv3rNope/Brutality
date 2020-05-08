#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include <d3d9.h>

namespace Hooks
{
	VALVE_SDK_INTERFACE_DECL(Util::Vmt::HookedMethod, oldReset);
	extern HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);
}
