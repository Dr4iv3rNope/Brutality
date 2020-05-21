#pragma once

namespace SourceSDK
{
	class INetMessage;

	enum class Flow
	{
		Outgoing, Incoming, Both,
	};

	class NetChannel final
	{
	public:
		void Shutdown(const char* reason);
		void SendNetMsg(INetMessage* message, bool force_reliable = false, bool voice = false);
		float GetLatency(Flow flow);
	};
}
