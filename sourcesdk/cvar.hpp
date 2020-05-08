#pragma once
#include "../valvesdk/interfaces.hpp"

#include "color.hpp"

namespace SourceSDK
{
	class ConVar;

	class CVar final
	{
	public:
		ConVar* FindVar(const char*);
	};

	VALVE_SDK_INTERFACE_DECL(CVar, cvar);
}
