#pragma once
struct ImDrawList;

namespace Features
{
	namespace TextRadar
	{
		extern void Update() noexcept;

		// call when menu is opened
		extern void DrawPreview(ImDrawList* list);

		extern void Draw(ImDrawList* list);
	}
}
