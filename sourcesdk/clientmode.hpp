#pragma once
#include <cstddef>

#include "../valvesdk/interfaces.hpp"

namespace SourceSDK
{
	class ClientModeShared final
	{
	public:
		std::size_t GetCreateMoveIndex();
		std::size_t GetOverrideViewIdx();
	};

	VALVE_SDK_INTERFACE_DECL(ClientModeShared, clientMode);
}