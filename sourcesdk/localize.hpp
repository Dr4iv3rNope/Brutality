#pragma once
#include "../valvesdk/interfaces.hpp"

namespace SourceSDK
{
	class LocalizedStringTable final
	{
	public:
		const wchar_t* Find(const char* token);
	};

	VALVE_SDK_INTERFACE_DECL(LocalizedStringTable, localize);
}
