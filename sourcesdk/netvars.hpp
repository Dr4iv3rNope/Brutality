#pragma once
#include "recv.hpp"
#include "clientclass.hpp"

#include "../util/stringhash.hpp"
#include "../util/xorstr.hpp"

#include <map>
#include <string>

namespace SourceSDK
{
	class NetvarManager final
	{
	private:
		std::map<Util::HashType, std::uint32_t> _offsets;

		void Parse(const char* client_class_name, const RecvTable&, std::uint32_t offset = 0);
		void Parse(const char* client_class_name, const RecvProp& prop, std::uint32_t offset = 0);

	public:
		NetvarManager();

		// @net_name:
		// hashed string format must be "TABLE_NAME:PROP_NAME"
		//
		// if prop not found, exception will be thrown
		std::size_t FindOffset(Util::HashType net_name);
	};

	extern NetvarManager* netvars;
}

#define SOURCE_SDK_NETVAR(typeName, funcName, tableName, propName, offset) \
	typeName& funcName() const { static const auto s_offset = SourceSDK::netvars->FindOffset(UTIL_HASH(tableName##":"##propName)); \
		return *(typeName*)(std::uintptr_t(this) + offset + s_offset); }
