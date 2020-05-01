#pragma once
struct ImDrawList;

namespace Features
{
	namespace Esp
	{
		void Update() noexcept;
		void DrawEntityListMenu();
		void Draw(ImDrawList* list);
	}
}
