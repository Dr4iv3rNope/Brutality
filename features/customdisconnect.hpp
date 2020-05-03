#pragma once
#include <string>

namespace Features
{
	namespace CustomDisconnect
	{
		void DrawMenu() noexcept;

		// call in FrameStageNotify -> Start
		void Think() noexcept;

		void Disconnect(const std::string& reason) noexcept;
	}
}
