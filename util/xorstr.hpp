//
// credits to https://github.com/KN4CK3R/XorStr
// fixed by me
//

#pragma once
#include <string>
#include <utility>

#ifdef _DEBUG
#define XORSTR_DISABLE
#endif

#pragma warning(disable: 6201)
#pragma warning(disable: 6294)

namespace Util
{
	#ifndef XORSTR_DISABLE
	namespace
	{
		constexpr int const_atoi(char c)
		{
			return c - '0';
		}
	}
	#endif

	template<typename _string_type, size_t _length>
	class _Basic_XorStr
	{
		using value_type = typename _string_type::value_type;
		static constexpr auto _length_minus_one = _length - 1;

	public:
		constexpr __forceinline _Basic_XorStr(value_type const (&str)[_length])
			: _Basic_XorStr(str, std::make_index_sequence<_length_minus_one>())
		{

		}

		inline auto c_str() const
		{
			#ifndef XORSTR_DISABLE
			decrypt();
			#endif

			return data;
		}

		inline auto str() const
		{
			#ifndef XORSTR_DISABLE
			decrypt();
			#endif

			return _string_type(data, data + _length_minus_one);
		}

		inline operator _string_type() const
		{
			return str();
		}

	private:
		template<size_t... indices>
		constexpr __forceinline _Basic_XorStr(value_type const (&str)[_length], std::index_sequence<indices...>)
			#ifndef XORSTR_DISABLE
			: data{ crypt(str[indices], indices)..., '\0' },
			encrypted(true)
			#else
			: data{ str[indices]... }
			#endif
		{

		}

		#ifndef XORSTR_DISABLE
		static constexpr auto XOR_KEY = static_cast<value_type>(
			const_atoi(__TIME__[7]) +
			const_atoi(__TIME__[6]) * 10 +
			const_atoi(__TIME__[4]) * 60 +
			const_atoi(__TIME__[3]) * 600 +
			const_atoi(__TIME__[1]) * 3600 +
			const_atoi(__TIME__[0]) * 36000
			);

		static __forceinline constexpr auto crypt(value_type c, size_t i)
		{
			// C4308 negative integral constant converted to unsigned type
			return static_cast<value_type>(c ^ (XOR_KEY + (value_type)i));
		}

		inline void decrypt() const
		{
			if (encrypted)
			{
				for (size_t t = 0; t < _length_minus_one; t++)
				{
					data[t] = crypt(data[t], t);
				}
				encrypted = false;
			}
		}
		#endif

		mutable value_type data[_length];

		#ifndef XORSTR_DISABLE
		mutable bool encrypted;
		#endif
	};
	//---------------------------------------------------------------------------
	template<size_t _length>
	using XorStrA = _Basic_XorStr<std::string, _length>;
	template<size_t _length>
	using XorStrW = _Basic_XorStr<std::wstring, _length>;
	template<size_t _length>
	using XorStrU16 = _Basic_XorStr<std::u16string, _length>;
	template<size_t _length>
	using XorStrU32 = _Basic_XorStr<std::u32string, _length>;
	//---------------------------------------------------------------------------
	template<typename _string_type, size_t _length, size_t _length2>
	inline auto operator==(const _Basic_XorStr<_string_type, _length>& lhs, const _Basic_XorStr<_string_type, _length2>& rhs)
	{
		static_assert(_length == _length2, "XorStr== different length");

		return _length == _length2 && lhs.str() == rhs.str();
	}
	//---------------------------------------------------------------------------
	template<typename _string_type, size_t _length>
	inline auto operator==(const _string_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
	{
		return lhs.size() == _length && lhs == rhs.str();
	}
	//---------------------------------------------------------------------------
	template<typename _stream_type, typename _string_type, size_t _length>
	inline auto& operator<<(_stream_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
	{
		lhs << rhs.c_str();

		return lhs;
	}
	//---------------------------------------------------------------------------
	template<typename _string_type, size_t _length, size_t _length2>
	inline auto operator+(const _Basic_XorStr<_string_type, _length>& lhs, const _Basic_XorStr<_string_type, _length2>& rhs)
	{
		return lhs.str() + rhs.str();
	}
	//---------------------------------------------------------------------------
	template<typename _string_type, size_t _length>
	inline auto operator+(const _string_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
	{
		return lhs + rhs.str();
	}
	//---------------------------------------------------------------------------
	template<size_t _length>
	constexpr __forceinline auto MakeXorStr(char const (&str)[_length])
	{
		return XorStrA<_length>(str);
	}
	//---------------------------------------------------------------------------
	template<size_t _length>
	constexpr __forceinline auto MakeXorStr(wchar_t const (&str)[_length])
	{
		return XorStrW<_length>(str);
	}
	//---------------------------------------------------------------------------
	template<size_t _length>
	constexpr __forceinline auto MakeXorStr(char16_t const (&str)[_length])
	{
		return XorStrU16<_length>(str);
	}
	//---------------------------------------------------------------------------
	template<size_t _length>
	constexpr __forceinline auto MakeXorStr(char32_t const (&str)[_length])
	{
		return XorStrU32<_length>(str);
	}
	//---------------------------------------------------------------------------
}

#define UTIL_XOR(string)	(::Util::MakeXorStr(string))
#define UTIL_CXOR(string)	(UTIL_XOR(string).c_str())
#define UTIL_SXOR(string)	(UTIL_XOR(string).str())
