#pragma once
#include "../util/vmt.hpp"

namespace SourceSDK
{
	struct ViewSetup;
}

namespace Hooks
{
	extern Util::Vmt::HookedMethod* oldOverrideView;
	extern void __stdcall OverrideView(SourceSDK::ViewSetup*);
}
