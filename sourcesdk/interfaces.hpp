#pragma once
#include <string>

#include "../util/xorstr.hpp"
#include "../util/pattern.hpp"
#include "../util/memory.hpp"

namespace SourceSDK
{
	enum class InterfaceStatus { Ok, Failed };

	using CreateInterfaceFn = void*(__cdecl*)(const char* name, InterfaceStatus* status);

	extern void* GetInterface(const std::string& module_name, const std::string& iface_name);
}

#define SOURCE_SDK_INTERFACE_DECL(typeName, varName) \
	extern typeName* const varName;

// Base interface declaration method
#define SOURCE_SDK_INTERFACE_BASE(typeName, varName, ...) \
	typeName* const varName { (typeName*)(__VA_ARGS__) }

// Declares interface and implements it using memory pattern scan
#define SOURCE_SDK_INTERFACE_PATTERN(typeName, varName, moduleName, aob, offset, followPtr) \
	SOURCE_SDK_INTERFACE_BASE(typeName, varName, \
		::Util::FollowPointer<std::uintptr_t, followPtr>( \
			UTIL_XFIND_PATTERN(moduleName, aob, offset) \
		) \
	)

// Default source sdk interface declaration
#define SOURCE_SDK_INTERFACE(typeName, varName, moduleName, ifaceName) \
	SOURCE_SDK_INTERFACE_BASE(typeName, varName, SourceSDK::GetInterface(UTIL_XOR(moduleName), UTIL_XOR(ifaceName)))
