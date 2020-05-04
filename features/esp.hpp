#pragma once
struct ImDrawList;

namespace Features
{
	namespace Esp
	{
		void Update() noexcept;
		void Draw(ImDrawList* list);

		void RegisterEntityListWindow() noexcept;
	}
}
