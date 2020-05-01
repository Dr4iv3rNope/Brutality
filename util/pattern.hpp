#pragma once
#include <cstdint>
#include <list>
#include <optional>
#include <string>

#include "xorstr.hpp"

namespace Util
{
	using Pattern = std::list<std::optional<std::byte>>;

	extern Pattern Aob(const std::string& pattern);
	extern std::wstring AobToWString(const Pattern& pattern);
	
	extern std::uintptr_t FindPattern(std::uintptr_t begin, std::uintptr_t end, const Pattern& bytes);
	extern std::uintptr_t FindPattern(const std::string& module_name, const Pattern& bytes);
}

// creates xor'd aob pattern
#define UTIL_XAOB(aob) Util::Aob(UTIL_XOR(aob))

// finds pattern in module
#define UTIL_XFIND_PATTERN(moduleName, aob, offset) \
	(Util::FindPattern(UTIL_XOR(moduleName), UTIL_XAOB(aob)) + offset)
