#pragma once
#include "sdk.hpp"

#include <cstddef>

namespace SourceSDK
{
	// aka ConCommandBase
	//
	// public/tier1/convar.h:99
	class IConCommand
	{
	public:
		virtual ~IConCommand() {}
	};

	// aka CCommand
	//
	// public/tier1/convar.h:181
	struct ConCmdArgs final
	{
	public:
		static constexpr int COMMAND_MAX_LENGTH = 512;
		static constexpr int COMMAND_MAX_ARG_COUNT = 64;

		int argCount;
		int firstArgLength;
		char argsBuffer[COMMAND_MAX_LENGTH];
		char argvBuffer[COMMAND_MAX_LENGTH];
		const char* args[COMMAND_MAX_ARG_COUNT];
	};

	// public/tier1/convar.h:261
	class ConCommand final : public IConCommand
	{
	public:
		#if SOURCE_SDK_IS_GMOD
		static constexpr std::size_t SIZE = 72;
		#endif

		using CallbackFn = void(*)(const ConCmdArgs&);
		using AutoCompleteFn = int(*)(const char*, void*);
	};

	ConCommand* CreateConCommand(const char* name, ConCommand::CallbackFn callback,
		const char* help_string = "", int flags = 0, ConCommand::AutoCompleteFn ac_callback = nullptr);

	class LinkedConCommand final
	{
	private:
		ConCommand* _concommand;

	public:
		inline LinkedConCommand(const char* name, ConCommand::CallbackFn callback,
			const char* help_string = "", int flags = 0, ConCommand::AutoCompleteFn ac_callback = nullptr)
		{
			this->_concommand = CreateConCommand(name, callback, help_string, flags, ac_callback);
		}

		inline ~LinkedConCommand()
		{
			delete _concommand;
		}

		inline ConCommand* operator->() noexcept
		{
			return this->_concommand;
		}
	};
}
