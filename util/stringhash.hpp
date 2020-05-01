#pragma once
#include "compiletime.hpp"

#include <string>

#define __UTIL_HASH_VALUE(hashVal, charVal, length, typeName) \
	(hashVal + charVal) * (length) * (sizeof(typeName))

namespace Util
{
	using HashType = unsigned long;

	//
	// compile time
	//

	namespace
	{
		template <typename T, unsigned szLen>
		constexpr __forceinline HashType MakeHash(const T(&str)[szLen], HashType hash, int cur_len) noexcept
		{
			return cur_len == -1
				? hash
				: MakeHash<T, szLen>(str, __UTIL_HASH_VALUE(hash, str[cur_len], szLen - 1, T), cur_len - 1);
		}
	}

	template <typename T, unsigned szLen>
	constexpr __forceinline auto MakeHash(const T(&str)[szLen]) noexcept
	{
		static_assert(szLen > 1, "length must be more than 1");

		return MakeHash<T, szLen>(str, 0, szLen - 1);
	}

	//
	// runtime
	//

	namespace
	{
		template <typename T>
		inline HashType MakeRuntimeHash(const std::basic_string<T>& str, HashType hash, int cur_len) noexcept
		{
			return cur_len == -1
				? hash
				: MakeRuntimeHash<T>(str, __UTIL_HASH_VALUE(hash, str[cur_len], str.size(), T), cur_len - 1);
		}
	}

	template <typename T>
	inline HashType MakeRuntimeHash(const std::basic_string<T>& str) noexcept
	{
		return MakeRuntimeHash<T>(str, 0, str.size() - 1);
	}
}

//
// compile-time
//

// calculates compile-time hash
#define UTIL_HASH(str) UTIL_FORCE_CONSTEXPR(Util::HashType, Util::MakeHash(str))

// calculates compile-time hash and
// converts it to std::string
#define UTIL_STR_HASH(str) std::to_string(UTIL_HASH(str))

// calculates compile-time hash and
// converts it to std::wstring
#define UTIL_WSTR_HASH(str) std::to_wstring(UTIL_HASH(str))


//
// run-time
//

// calculates run-time hash
#define UTIL_RUNTIME_HASH(str) Util::MakeRuntimeHash(str)

// calculates run-time hash and
// converts it to std::string
#define UTIL_RUNTIME_STR_HASH(str) std::to_string(UTIL_RUNTIME_HASH(str))

// calculates run-time hash and
// converts it to std::wstring
#define UTIL_RUNTIME_WSTR_HASH(str) std::to_wstring(UTIL_RUNTIME_HASH(str))
