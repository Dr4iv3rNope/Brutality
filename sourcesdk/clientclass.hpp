#pragma once
#include "sdk.hpp"
#include "recv.hpp"

namespace SourceSDK
{
	using ClassId = int;

	struct ClientClass final
	{
	private:
		const void* createFunction;
		const void* createEventFunction;

	public:
		const char* networkName;
		const RecvTable& recvTable;
		const ClientClass* next; // can be null!
		ClassId id;

		bool IsPlayer() const;
	};
}
