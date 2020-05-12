#pragma once
#include "buttoncode.hpp"

namespace SourceSDK
{
	class InputSystem
	{
	public:
		void EnableInput(bool enable);
		bool IsButtonDown(ButtonCode code);
		void ResetInputState();
	};
}
