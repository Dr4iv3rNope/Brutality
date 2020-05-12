#pragma once

namespace SourceSDK
{
	class LocalizedStringTable final
	{
	public:
		const wchar_t* Find(const char* token);
	};
}
