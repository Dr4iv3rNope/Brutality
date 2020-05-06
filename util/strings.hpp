#pragma once
#include <string>
#include <sstream>
#include <algorithm>

namespace Util
{
	extern std::wstring ToWideChar(const std::string& str);
	extern std::string ToMultiByte(const std::wstring& str);

	template <typename T>
	inline std::basic_string<T> ToLower(std::basic_string<T> str) noexcept
	{
		std::transform(str.begin(), str.end(), str.begin(), [] (T ch)
		{
			return std::tolower(ch);
		});

		return str;
	}
}

#define UTIL_FORMAT_EX(str_type, fmt) ((std::basic_stringstream<str_type::value_type>{} << fmt).str()) // formats string using stringstream
	// example: UTIL_FORMAT_EX(std::string, "100 in hex is " << std::hex << 100);

#define UTIL_FORMAT(fmt) UTIL_FORMAT_EX(std::string, fmt)	// formats multibyte string using stringstream
	// example: UTIL_FORMAT("100 + 200 = " << (100 + 200));

#define UTIL_WFORMAT(fmt) UTIL_FORMAT_EX(std::wstring, fmt)	// formats widechar string using stringstream
	// example: UTIL_WFORMAT(L"this is formated " << 200);

#define UTIL_U16FORMAT(fmt) UTIL_FORMAT_EX(std::u16string, fmt)	// formats u16 string using stringstream
	// example: UTIL_U32FORMAT(u"char16_t example");

#define UTIL_U32FORMAT(fmt) UTIL_FORMAT_EX(std::u32string, fmt)	// formats u32 string using stringstream
	// example: UTIL_U32FORMAT(U"char32_t example");