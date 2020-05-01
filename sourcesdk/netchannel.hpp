#pragma once

namespace SourceSDK
{
	class INetMessage;

	class NetChannel final
	{
	public:
		void Shutdown(const char* reason);
		void SendNetMsg(INetMessage* message, bool force_reliable = false, bool voice = false);
	};
}
