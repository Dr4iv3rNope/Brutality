#pragma once
#include <cstddef>

namespace SourceSDK
{
	class ClientModeShared final
	{
	public:
		std::size_t GetCreateMoveIndex();
		std::size_t GetOverrideViewIdx();
	};
}