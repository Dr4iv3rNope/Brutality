#pragma once
#include "../valvesdk/interfaces.hpp"

namespace SourceSDK
{
	class ViewRender final
	{
	public:
		std::size_t GetRenderViewIndex();
	};

	VALVE_SDK_INTERFACE_DECL(ViewRender, view);
}
