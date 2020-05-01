#include "endscene.hpp"

#include "../shutdown.hpp"

#include "../ui/ui.hpp"

HRESULT __stdcall Hooks::EndScene(IDirect3DDevice9* device)
{
	CREATE_SHUTDOWN_HOOK_GUARD(L"EndScene", oldEndScene);
	MAKE_BUSY_SHUTDOWN_GUARD;

	UI::Think();

	return reinterpret_cast<decltype(Hooks::EndScene)*>
		(oldEndScene->GetOriginal())
		(device);
}
