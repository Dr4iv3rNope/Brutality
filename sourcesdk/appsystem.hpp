#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	class IAppSystem
	{
	public:
		enum class InitReturnVal : int { Failed, Ok };

		virtual bool Connect(CreateInterfaceFn) = 0;
		virtual void Disconnect() = 0;
		virtual void* QueryInterface(const char* pInterfaceName) = 0;
		virtual InitReturnVal Init() = 0;
		virtual void Shutdown() = 0;
	};
}
