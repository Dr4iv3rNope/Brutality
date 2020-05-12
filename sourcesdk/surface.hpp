#pragma once
#include <cstddef>

namespace SourceSDK
{
	enum class CursorType
	{
		User, None, Arrow, IBeam, HourGlass,
		WaitArrow, Crosshair, Up,
		SizeNWSE, SizeNESW, SizeWE, SizeNS, SizeAll,
		No, Hand, Blank
	};

	class MatSystemSurface final
	{
	public:
		bool IsCursorLocked();
		void UnlockCursor();

		std::size_t GetLockCursorIndex();
		std::size_t GetSetCursorIndex();
		std::size_t GetPaintTraverseExIndex();
	};
}
