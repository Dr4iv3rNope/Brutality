#pragma once
#include "../valvesdk/interfaces.hpp"

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

	VALVE_SDK_INTERFACE_DECL(InputSystem, inputsystem);
}
