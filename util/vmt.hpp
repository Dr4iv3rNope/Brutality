#pragma once
#include "pattern.hpp"

#include <cstddef>

namespace Util
{
	namespace Vmt
	{
		inline void** GetVmt(void* object) noexcept
		{
			return *(void***)object;
		}

		inline void* GetMethod(void* object, std::size_t idx) noexcept
		{
			return ((void***)object)[idx];
		}

		template <typename T, typename ...A>
		constexpr inline T CallMethod(void* object, std::size_t idx, A ...args) noexcept
		{
			#ifdef _X64
			return ((*reinterpret_cast<T(__fastcall***)(const void*, A...)>(object))[idx])(object, args...);
			#else
			return ((*reinterpret_cast<T(__thiscall***)(const void*, A...)>(object))[idx])(object, args...);
			#endif
		}

		class HookedMethod
		{
		private:
			void** _region; // address where pointer has been replaced
			void* _original; // pointer to original function
			bool initialized { false };

		public:
			HookedMethod(void* object, std::size_t idx);
			~HookedMethod();

			void Initialize(const void* new_func);
			void Shutdown() noexcept;

			// returns original function
			const void* GetOriginal() const;
		};

		// deprecated and unsafe
		// replaces pointer to method in vmt
		//extern const void* Replace(const void* object, std::size_t idx, const void* new_func);

		// finds method in vmt using pattern
		extern std::size_t FindMethod(void* object, const Pattern& bytes, std::size_t cc_count = 1);

		// calculates vmt methods count
		extern std::size_t GetCount(void* object);
	}
}

// finds vmt method
#define VMT_XFIND_METHOD_EX(object, aob, ...) \
	Util::Vmt::FindMethod(object, UTIL_XAOB(aob), __VA_ARGS__)

// finds vmt method in this object
#define VMT_XFIND_METHOD(aob, ...) VMT_XFIND_METHOD_EX(this, aob, __VA_ARGS__)
