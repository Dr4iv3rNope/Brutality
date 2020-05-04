#pragma once
#include <string>

namespace Features
{
	namespace CustomDisconnect
	{
		void RegisterWindow() noexcept;

		// call in FrameStageNotify -> Start
		void Think() noexcept;

		void Disconnect(const std::string& reason) noexcept;
	}
}
