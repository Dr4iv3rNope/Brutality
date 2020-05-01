#pragma once
#include <list>

namespace Util
{
	namespace Shellcode
	{
		using Bytes = std::list<char>;

		class Instruction
		{
		public:
			virtual bool IsOpcode() const = 0;

			virtual std::size_t GetSize() const = 0;
			virtual void CopyTo(Bytes& buffer, std::size_t offset) const = 0;
		};

		class Address : public Instruction
		{
		private:
			std::uintptr_t _address;

		public:
			inline Address(std::uintptr_t abs_address) noexcept
				: _address { abs_address } {}

			virtual bool IsOpcode() const override { return false; }		
			virtual std::size_t GetSize() const override { return sizeof(std::uintptr_t); }
			virtual void CopyTo(Bytes& buffer, std::size_t offset) const override;
		
			inline void SetAddress(std::uintptr_t address) noexcept { _address = address; }
			inline std::uintptr_t GetAddress() noexcept { return _address; }
		};

		class Opcode : public Instruction
		{
		private:
			std::list<char> _opcode;

		public:
			inline Opcode(std::list<char> opcode) noexcept
				: _opcode { opcode } {}

			virtual bool IsOpcode() const override { return true; }
			virtual std::size_t GetSize() const override { return _opcode.size(); }
			virtual void CopyTo(Bytes& buffer, std::size_t offset) const override;
		};

		class Builder final
		{
		private:
			std::list<Instruction*> _instructions;

		public:
			~Builder();

			inline void PushOpcode(Bytes opcode) noexcept
			{
				_instructions.push_back(new Opcode(opcode));
			}

			inline void PushAddress(std::uintptr_t address) noexcept
			{
				_instructions.push_back(new Address(address));
			}

			Bytes Build(std::uintptr_t shell_code_addr) const;
		};
	}
}
