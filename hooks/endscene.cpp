#include "endscene.hpp"

#include "../shutdown.hpp"

#include "../ui/ui.hpp"

HRESULT __stdcall Hooks::EndScene(IDirect3DDevice9* device)
{
	_SHUTDOWN_GUARD;

	UI::Draw();

	return reinterpret_cast<decltype(Hooks::EndScene)*>
		(hooks->oldEndScene->GetOriginal())
		(device);
}
