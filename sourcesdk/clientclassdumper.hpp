#pragma once
#include "../valvesdk/interfaces.hpp"

#include "clientclass.hpp"

#include <string>
#include <map>

namespace SourceSDK
{
	class ClientClassDumper final
	{
	private:
		std::map<std::string, const ClientClass*> _clientclasses;

	public:
		ClientClassDumper();

		const ClientClass* FindClientClass(const std::string& cs_name);
	};

	extern ClientClassDumper* clientClassDumper;
}
