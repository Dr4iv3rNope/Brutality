#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	class ViewRender final
	{
	public:
		std::size_t GetRenderViewIndex();
	};

	SOURCE_SDK_INTERFACE_DECL(ViewRender, view);
}
