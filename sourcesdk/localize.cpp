#include "localize.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"

const wchar_t* SourceSDK::LocalizedStringTable::Find(const char* token)
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("8B F1 8B 06 FF 50 ?? 83 F8 FF 75 ?? 33 C0 5E")
	};
	#endif

	return Util::Vmt::CallMethod<const wchar_t*, const char*>(this, offset, token);
}
