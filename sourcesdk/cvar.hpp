#pragma once
#include "interfaces.hpp"
#include "color.hpp"

namespace SourceSDK
{
	class ConVar;

	class CVar final
	{
	public:
		ConVar* FindVar(const char*);
	};

	SOURCE_SDK_INTERFACE_DECL(CVar, cvar);
}
