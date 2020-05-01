#pragma once

namespace UI
{
	extern bool IsInitialized() noexcept;
	
	extern void Initialize();
	extern void Shutdown() noexcept;
	
	extern void Draw();
	extern void Reset();

	inline void Think()
	{
		if (!IsInitialized())
			Initialize();

		Draw();
	}
}
