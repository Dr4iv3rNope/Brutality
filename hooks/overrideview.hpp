#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

namespace SourceSDK
{
	struct ViewSetup;
}

namespace Hooks
{
	VALVE_SDK_INTERFACE_DECL(Util::Vmt::HookedMethod, oldOverrideView);
	extern void __stdcall OverrideView(SourceSDK::ViewSetup*);
}
