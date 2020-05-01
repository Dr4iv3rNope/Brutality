#pragma once

namespace Features
{
	namespace CustomDisconnect
	{
		void DrawMenu() noexcept;

		bool Disconnect(const char* reason) noexcept;
	}
}
