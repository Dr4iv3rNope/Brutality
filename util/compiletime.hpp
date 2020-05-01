#pragma once

// only available in msvc
#ifdef _MSC_VER
namespace Util
{
	namespace MSVC
	{
		// for some reason, msvc dont gives a fuck
		// and dont compile functions as regular
		//
		// so we use that thing
		//
		// also credits:
		// https://stackoverflow.com/questions/37034020/why-constexpr-is-not-evaluated-at-compile-time-msvc-2015
		template <typename T, T Value>
		struct CompileTime
		{
			enum : T
			{
				result = Value
			};
		};
	}
}

#define UTIL_FORCE_CONSTEXPR(typeName, constexprValue) (Util::MSVC::CompileTime<typeName, constexprValue>::result)
#else
#define UTIL_FORCE_CONSTEXPR(typeName, constexprValue) constexprValue
#endif
