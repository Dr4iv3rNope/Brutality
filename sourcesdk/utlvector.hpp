#pragma once
#include "utlmemory.hpp"

namespace SourceSDK
{
	template <typename T, typename A = UtlMemory<T>>
	class UtlVector
	{
	private:
		A _memory;
		int _size;
		T* _elements;
	};
}
