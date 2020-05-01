#pragma once
#include "netmessage.hpp"

namespace SourceSDK
{
	class NetSetConVar : public INetMessage
	{
	public:
		static NetSetConVar* Create(const char* name, const char* value);
	};

	extern bool SendConVarValue(const char* name, const char* value);
}
