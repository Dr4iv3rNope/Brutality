#pragma once
struct ImDrawList;

namespace Features
{
	namespace SpectatorList
	{
		extern void Update() noexcept;

		// call when menu is opened
		extern void DrawPreview(ImDrawList* list);

		extern void Draw(ImDrawList* list);
	}
}
