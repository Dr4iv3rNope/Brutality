#pragma once
#include "interfaces.hpp"
#include "clientclass.hpp"

namespace SourceSDK
{
	class HLClient final
	{
	public:
		std::size_t GetFrameStageNotifyIndex();
		std::size_t GetRenderViewIndex();

		const ClientClass* GetHeadClientClass();
	};

	SOURCE_SDK_INTERFACE_DECL(HLClient, clientDLL);
}
