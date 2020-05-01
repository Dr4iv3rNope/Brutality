#include "asmhook.hpp"
#include "memory.hpp"

#include "debug/labels.hpp"

#include <Windows.h>

Util::AsmHook::AsmHook(std::uintptr_t _where, std::size_t hook_size)
{
	UTIL_DEBUG_ASSERT(hook_size >= 5);

	_replacedBytes = std::list<char>((char*)_where, (char*)(_where + hook_size));

	this->_region = _where;
	this->_backAddress = _where + hook_size;
}

constexpr int ASM_NOP { 0x90 };
constexpr int ASM_JMP { 0xE9 };

void Util::AsmHook::Initialize(const void* jmp_to)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Initializing assembler hook"));
	UTIL_DEBUG_ASSERT(!_initialized);
	UTIL_DEBUG_ASSERT(!_shellCode);

	{
		DWORD temp, old_prot;

		if (VirtualProtect((void*)this->_region, _replacedBytes.size(), PAGE_EXECUTE_READWRITE, &old_prot))
		{
			memset((void*)_region, ASM_NOP, _replacedBytes.size());

			*(char*)_region = (char)ASM_JMP;
			*(std::uintptr_t*)(_region + 1) = Util::GetRelAddress(_region, std::uintptr_t(jmp_to));

			VirtualProtect((void*)this->_region, _replacedBytes.size(), old_prot, &temp);
		}
		else
			UTIL_LABEL_FAIL();
	}

	#ifdef _DEBUG
	_initialized = true;
	#endif
	UTIL_LABEL_OK();
}

void Util::AsmHook::InitializeShellCode(Shellcode::Bytes shell)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Creating shell code"));
	UTIL_DEBUG_ASSERT(!_initialized);
	UTIL_DEBUG_ASSERT(!_shellCode);

	{
		DWORD temp, old_prot;

		if (VirtualProtect((void*)this->_region, _replacedBytes.size(), PAGE_EXECUTE_READWRITE, &old_prot))
		{
			UTIL_LABEL_ENTRY_EX(0, UTIL_XOR(L"Filling shell code"));


			_shellCodeSize = _replacedBytes.size() + shell.size() + 5;
			_shellCode = new char[_shellCodeSize - 1] { };

			std::copy(shell.begin(), shell.end(), _shellCode);
			std::copy(_replacedBytes.begin(), _replacedBytes.end(), _shellCode + shell.size());

			_shellCode[_shellCodeSize - 1 - 5] = (char)ASM_JMP;
			*(std::uintptr_t*)& _shellCode[_shellCodeSize - 1 - 4] =
				Util::GetRelAddress((std::uintptr_t)&_shellCode[_shellCodeSize - 1 - 5], GetBackAddress());

			if (!VirtualProtect(_shellCode, _shellCodeSize, PAGE_EXECUTE_READWRITE, &temp))
				UTIL_LABEL_FAIL_EX(0);
			else 
				UTIL_LABEL_OK_EX(0);


			memset((void*)_region, ASM_NOP, _replacedBytes.size());
			*(char*)_region = (char)ASM_JMP;
			*(std::uintptr_t*)(_region + 1) = Util::GetRelAddress(_region, std::uintptr_t(_shellCode));


			VirtualProtect((void*)this->_region, _replacedBytes.size(), old_prot, &temp);
		}
		else
			UTIL_LABEL_FAIL();
	}

	#ifdef _DEBUG
	_initialized = true;
	#endif
	UTIL_LABEL_OK();
}

Util::AsmHook::~AsmHook()
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown assembler hook"));
	UTIL_DEBUG_ASSERT(_initialized);

	{
		DWORD temp, old_prot;

		if (VirtualProtect((void*)this->_region, _replacedBytes.size(), PAGE_EXECUTE_READWRITE, &old_prot))
		{
			std::copy(_replacedBytes.begin(), _replacedBytes.end(), (char*)_region);

			VirtualProtect((void*)this->_region, _replacedBytes.size(), old_prot, &temp);
		}
		else
			UTIL_LABEL_FAIL();
	}

	if (_shellCode)
		delete[] _shellCode;

	UTIL_LABEL_OK();
}
