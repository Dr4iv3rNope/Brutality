#pragma once
#include "../util/vmt.hpp"

#include <d3d9.h>

namespace Hooks
{
	extern Util::Vmt::HookedMethod* oldEndScene;
	extern HRESULT __stdcall EndScene(IDirect3DDevice9*);
}
