#pragma once
#include "sdk.hpp"
#include "interfaces.hpp"
#include "vector.hpp"
#include "buttoncode.hpp"

namespace SourceSDK
{
	struct UserCmd;

	class Input
	{
	public:
		UserCmd* GetUserCmd(int sequence_number);
	};

	SOURCE_SDK_INTERFACE_DECL(Input, input);
}
