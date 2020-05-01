#pragma once
#include "shellcodebuilder.hpp"

namespace Util
{
	class AsmHook final
	{
	private:
		std::uintptr_t _region;
		std::list<char> _replacedBytes;
		std::uintptr_t _backAddress;

		#ifdef _DEBUG
		bool _initialized { false };
		#endif

		char* _shellCode { nullptr };
		std::size_t _shellCodeSize { 0 };

	public:
		AsmHook(std::uintptr_t _where, std::size_t hook_size);
		~AsmHook();

		void Initialize(const void* jmp_to);

		inline void InitializeShellCode(const Shellcode::Builder& builder)
		{
			InitializeShellCode(builder.Build(_region));
		}

		void InitializeShellCode(Shellcode::Bytes shell);
		
		inline const auto& GetReplacedBytes() const noexcept { return _replacedBytes; }
		inline auto GetRegion() const noexcept { return _region; }
		inline auto GetBackAddress() const noexcept { return _backAddress; }
	};
}

#define UTIL_NAKED_FUNC(funcName) \
	__declspec(naked) void funcName()
