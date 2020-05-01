#pragma once
struct ImDrawList;

namespace Features
{
	namespace Crosshair
	{
		void DrawPreview(ImDrawList* list) noexcept;
		void Draw(ImDrawList* list, bool isMenuOpen) noexcept;
	}
}
