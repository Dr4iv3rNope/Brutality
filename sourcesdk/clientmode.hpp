#pragma once
#include <cstddef>

#include "interfaces.hpp"

namespace SourceSDK
{
	class ClientModeShared final
	{
	public:
		std::size_t GetCreateMoveIndex();
		std::size_t GetOverrideViewIdx();
	};

	SOURCE_SDK_INTERFACE_DECL(ClientModeShared, clientMode);
}