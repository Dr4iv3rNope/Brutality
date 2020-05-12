#include "endscene.hpp"

#include "../shutdown.hpp"

#include "../ui/ui.hpp"

HRESULT __stdcall Hooks::EndScene(IDirect3DDevice9* device)
{
	SHUTDOWN_HOOK_GUARD(L"EndScene");
	MAKE_BUSY_SHUTDOWN_GUARD;

	UI::Think();

	return reinterpret_cast<decltype(Hooks::EndScene)*>
		(hooks->oldEndScene->GetOriginal())
		(device);
}
