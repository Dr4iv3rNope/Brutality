#pragma once
#include "../util/vmt.hpp"

#include <d3d9.h>

namespace Hooks
{
	extern Util::Vmt::HookedMethod* oldReset;
	extern HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);
}
