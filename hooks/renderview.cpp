#include "renderview.hpp"

#include "../shutdown.hpp"

#include "../sourcesdk/render.hpp"

#include <mutex>

static inline SourceSDK::VMatrix& LastWorldToScreenMatrix() noexcept
{
	static SourceSDK::VMatrix worldToScreenMatrix;

	return worldToScreenMatrix;
}

static inline std::mutex& GetLastWorldToScreenMutex() noexcept
{
	static std::mutex lastWorldToScreenMutex;

	return lastWorldToScreenMutex;
}

void __fastcall Hooks::RenderView(void* ecx, void* edx, const SourceSDK::ViewSetup& setup, int clearFlags, int whatToDraw)
{
	_SHUTDOWN_GUARD;

	reinterpret_cast<decltype(RenderView)*>
		(hooks->oldRenderView->GetOriginal())
		(ecx, edx, setup, clearFlags, whatToDraw);

	GetLastWorldToScreenMutex().lock();
	memcpy(LastWorldToScreenMatrix().data(), interfaces->render->WorldToScreenMatrix().data(), sizeof(SourceSDK::VMatrix));
	GetLastWorldToScreenMutex().unlock();
}

void Hooks::CopyLastWorldToScreenMatrix(SourceSDK::VMatrix& matrix) noexcept
{
	GetLastWorldToScreenMutex().lock();
	memcpy(matrix.data(), LastWorldToScreenMatrix().data(), sizeof(matrix));
	GetLastWorldToScreenMutex().unlock();
}
