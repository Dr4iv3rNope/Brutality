#pragma once

namespace SourceSDK
{
	struct UserCmd;
}

namespace Features
{
	namespace TriggerBot
	{
		extern void Think(SourceSDK::UserCmd* cmd) noexcept;
	}
}
