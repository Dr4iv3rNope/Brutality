#pragma once
#include "../util/vmt.hpp"

namespace SourceSDK
{
	struct ViewSetup;
}

namespace Hooks
{
	extern void __stdcall OverrideView(SourceSDK::ViewSetup*);
}
