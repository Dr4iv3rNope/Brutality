#pragma once
#include "../valvesdk/interfaces.hpp"

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

	VALVE_SDK_INTERFACE_DECL(HLClient, clientDLL);
}
