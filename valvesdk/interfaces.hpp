#pragma once
#include <string>

#include "../util/xorstr.hpp"
#include "../util/pattern.hpp"
#include "../util/memory.hpp"

namespace ValveSDK
{
	enum class InterfaceStatus { Ok, Failed };

	template <typename T = void*>
	using CreateInterfaceFn = T(__cdecl*)(const char* name, InterfaceStatus* status);

	extern void* GetInterface(const std::string& module_name, const std::string& iface_name) noexcept;
}

// Base interface declaration method
#define VALVE_SDK_INTERFACE_IMPL_EX(typeName, varName, ...) \
	varName = (typeName)(__VA_ARGS__)

// Declares interface and implements it using memory pattern scan
#define VALVE_SDK_INTERFACE_IMPL_PATTERN(varName, moduleName, aob, offset, followPtr) \
	VALVE_SDK_INTERFACE_IMPL_EX(decltype(varName), varName, \
		::Util::FollowPointer<std::uintptr_t, followPtr>( \
			UTIL_XFIND_PATTERN(moduleName, aob, offset) \
		) \
	)

#define VALVE_SDK_INTERFACE_IMPL(varName, moduleName, ifaceName) \
	VALVE_SDK_INTERFACE_IMPL_EX(decltype(varName), varName, ValveSDK::GetInterface(UTIL_XOR(moduleName), UTIL_XOR(ifaceName)))
