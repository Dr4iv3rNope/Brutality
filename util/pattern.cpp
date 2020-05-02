#include "pattern.hpp"
#include "strings.hpp"
#include "stringhash.hpp"

#include "debug/errors.hpp"
#include "debug/assert.hpp"

#include <stdexcept>
#include <sstream>

#include <Windows.h>
#include <Psapi.h>

Util::Pattern Util::Aob(const std::string& pattern)
{
	UTIL_DEBUG_ASSERT(!pattern.empty());

	Util::Pattern bytes{};

	{
		std::size_t begin{ std::string::npos }, end{ std::string::npos };

		do
		{
			end = pattern.find(' ', begin + 1);

			{
				const auto bytes_str = pattern.substr(begin + 1, end - begin - 1);

				UTIL_DEBUG_ASSERT(!bytes_str.empty());

				if (bytes_str[0] == '?')
					bytes.push_back(std::nullopt);
				else
				{
					const std::uint32_t bytes_val = std::stoi(bytes_str, nullptr, 16);

					// stoi out of range
					UTIL_DEBUG_ASSERT(0xFF >= bytes_val);

					bytes.push_back(std::byte(bytes_val));
				}
			}

			begin = end;
		}
		while (begin != std::string::npos && end != std::string::npos);
	}

	UTIL_DEBUG_ASSERT(bytes.size() != 0);

	#ifdef _DEBUG
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Created pattern \"") <<
		Util::ToWString(pattern) <<
		UTIL_XOR(L"\" (") <<
		UTIL_RUNTIME_HASH(pattern) <<
		UTIL_XOR(L") got UID ") <<
		UTIL_RUNTIME_HASH(AobToWString(bytes))
	));
	#else
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Created pattern ") <<
		UTIL_RUNTIME_HASH(pattern) <<
		UTIL_XOR(L" got UID ") <<
		UTIL_RUNTIME_HASH(AobToWString(bytes))
	));
	#endif

	return bytes;
}

std::wstring Util::AobToWString(const Pattern& pattern)
{
	std::wstringstream result;
	result << std::hex;

	for (auto i = pattern.begin(); i != pattern.end(); i++)
	{
		if ((*i).has_value())
			result << int((*i).value());
		else
			result << UTIL_SXOR(L"??");

		if (i != pattern.end())
			result << ' ';
	}

	return result.str();
}

std::uintptr_t Util::FindPattern(std::uintptr_t begin, std::uintptr_t end, const Pattern& bytes)
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Trying to find pattern ") <<
		UTIL_RUNTIME_HASH(Util::AobToWString(bytes)) <<
		UTIL_XOR(L" at ") << std::hex <<
		begin << '-' << end
	));
	
	end -= bytes.size();

	if (begin >= end)
	{
		UTIL_RUNTIME_ERROR("@begin is more or equal to @end");
	}
	else
	{
		const auto iter_begin = bytes.cbegin();
		const auto iter_end = bytes.cend();
		auto iter = iter_begin;

		while (begin != end)
		{
			{
				auto begin_offset = begin;

				while (iter != iter_end)
				{
					{
						const auto& data = *iter;

						if (data.has_value() && data.value() != *reinterpret_cast<const std::byte*>(begin_offset))
							goto NOT_EQ;
					}

					iter++;
					begin_offset++;
				}
			}

			UTIL_LOG(UTIL_WFORMAT(UTIL_XOR(L"Found pattern at 0x") << std::hex << begin));
			return begin;

		NOT_EQ:
			iter = iter_begin;
			begin++;
		}
	}

	UTIL_RUNTIME_ERROR("Failed to find pattern!");
}

std::uintptr_t Util::FindPattern(const std::string& module_name, const Pattern& bytes)
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Trying to find pattern in module ") << ToWString(module_name)
	));

	auto module = GetModuleHandleA(module_name.c_str());
	UTIL_ASSERT(module, "Failed to get module");

	MODULEINFO moduleInfo;

	UTIL_ASSERT(
		::K32GetModuleInformation(::GetCurrentProcess(), module, &moduleInfo, sizeof(moduleInfo)),
		"Failed to get module information"
	)

	return FindPattern(
		std::uintptr_t(moduleInfo.lpBaseOfDll),
		std::uintptr_t(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage,
		bytes
	);
}
