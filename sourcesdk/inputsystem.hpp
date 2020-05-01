#pragma once
#include "interfaces.hpp"
#include "buttoncode.hpp"

namespace SourceSDK
{
	class CInputSystem
	{
	public:
		void EnableInput(bool enable);
		bool IsButtonDown(ButtonCode code);
		void ResetInputState();
	};

	SOURCE_SDK_INTERFACE_DECL(CInputSystem, inputsystem);
}
