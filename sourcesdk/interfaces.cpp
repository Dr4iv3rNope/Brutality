#include "interfaces.hpp"

#include "../util/debug/labels.hpp"
#include "../util/strings.hpp"

#include <Windows.h>
#include <stdexcept>

void* SourceSDK::GetInterface(const std::string& module_name, const std::string& iface_name)
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Finding interface: ") <<
		Util::ToWideChar(module_name) << ' ' <<
		Util::ToWideChar(iface_name)
	));

	auto hmodule = GetModuleHandleA(module_name.c_str());
	UTIL_ASSERT(hmodule, "Failed to get module");

	const auto createInterface =
		(void* const(*__cdecl)(const char*, int*))
		GetProcAddress(hmodule, UTIL_CXOR("CreateInterface"));
	UTIL_ASSERT(createInterface, "Failed to get CreateInterface function");

	auto iface = createInterface(iface_name.c_str(), nullptr);
	UTIL_ASSERT(iface, "Failed to get interface pointer");

	UTIL_LOG(UTIL_WFORMAT(UTIL_XOR(L"Found at 0x") << std::hex << int(iface)));
	return iface;
}
