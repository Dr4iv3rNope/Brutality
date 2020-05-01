#pragma once
#include "../util/debug/assert.hpp"

namespace SourceSDK
{
	template <typename T>
	class UtlMemory
	{
	private:
		T* _memory;
		int _allocCount;
		int _growSize;

	public:
		inline bool IsIdxValid(int idx) const noexcept
		{
			return idx > 0 && idx < this->_allocCount;
		}

		inline T& operator[](int idx) noexcept
		{
			UTIL_DEBUG_ASSERT(this->IsIdxValid(idx));

			return this->_memory[idx];
		}

		inline const T& operator[](int idx) const noexcept
		{
			UTIL_DEBUG_ASSERT(this->IsIdxValid(idx));

			return this->_memory[idx];
		}
	};
}
