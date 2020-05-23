#include "shellcodebuilder.hpp"
#include "memory.hpp"

#include <algorithm>

#include "debug/assert.hpp"

void Util::Shellcode::Address::CopyTo(Bytes& buffer, std::size_t offset) const
{
	auto where = buffer.begin();
	std::advance(where, offset);

	std::copy(
		(char*)&_address,
		((char*)&_address) + sizeof(std::uintptr_t),
		where
	);
}

void Util::Shellcode::Opcode::CopyTo(Bytes& buffer, std::size_t offset) const
{
	auto where = buffer.begin();
	std::advance(where, offset);

	std::copy(_opcode.begin(), _opcode.end(), where);
}

Util::Shellcode::Builder::~Builder()
{
	for (auto& instruction : _instructions)
		delete instruction;
}

Util::Shellcode::Bytes Util::Shellcode::Builder::Build(std::uintptr_t shell_code_addr) const
{
	Bytes shell;

	std::uintptr_t prev_offset = 0;
	for (auto inst : _instructions)
	{
		auto offset = shell.size();

		shell.resize(offset + inst->GetSize());

		if (inst->IsOpcode())
			inst->CopyTo(shell, offset);
		else
		{
			auto addr = (Address*)inst;

			addr->SetAddress(
				Util::GetRelAddress(
					shell_code_addr + prev_offset,
					addr->GetAddress()
				)
			);

			addr->CopyTo(shell, offset);
		}

		prev_offset += offset;
	}

	return shell;
}
