#pragma once
#include "color.hpp"

namespace SourceSDK
{
	class ConVar;

	class CVar final
	{
	public:
		ConVar* FindVar(const char*);
	};
}
