#pragma once
struct ImDrawList;

namespace Features
{
	namespace TextRadar
	{
		void Update() noexcept;

		// call when menu is opened
		void DrawPreview(ImDrawList* list);

		void Draw(ImDrawList* list);
	}
}
