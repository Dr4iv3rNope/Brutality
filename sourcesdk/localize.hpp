#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	class LocalizedStringTable final
	{
	public:
		const wchar_t* Find(const char* token);
	};

	SOURCE_SDK_INTERFACE_DECL(LocalizedStringTable, localize);
}
