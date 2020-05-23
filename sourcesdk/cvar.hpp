#pragma once
#include "color.hpp"

namespace SourceSDK
{
	class IConVar;

	class CVar final
	{
	public:
		IConVar* FindVar(const char*);
	};
}
