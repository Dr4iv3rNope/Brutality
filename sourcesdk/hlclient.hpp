#pragma once
#include <cstddef>

#include "clientclass.hpp"

namespace SourceSDK
{
	class HLClient final
	{
	public:
		std::size_t GetFrameStageNotifyIndex();
		std::size_t GetCreateMoveIndex();

		const ClientClass* GetHeadClientClass();
	};
}
