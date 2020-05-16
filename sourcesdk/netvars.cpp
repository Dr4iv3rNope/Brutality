#include "netvars.hpp"
#include "hlclient.hpp"

#include "../main.hpp"

#include "../util/debug/assert.hpp"
#include "../util/debug/labels.hpp"
#include "../util/debug/errors.hpp"

#include "../util/strings.hpp"

#include <stdexcept>
#include <fstream>

// dump everything in file?
//#define SOURCE_SDK_NETVARS_MGR_DUMP UTIL_XOR("Z:\\Documents\\dump1.txt")

#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
std::ofstream* dumpFile{ nullptr };
#endif

void SourceSDK::NetvarManager::Parse(const char* client_class_name, const RecvTable& table, std::uint32_t offset)
{
	UTIL_DEBUG_ASSERT(client_class_name);

	#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
	*dumpFile
		<< "[TABLE TREE] Address: 0x"
		<< std::hex << &table << std::dec
		<< " Name: \""
		<< table.netTableName
		<< "\" Prop Count: "
		<< table.propCount
		<< std::endl;
	#endif

	for (auto i = 0; i < table.propCount; i++)
		Parse(client_class_name, table.propArray[i], offset);
}

void SourceSDK::NetvarManager::Parse(const char* client_class_name, const RecvProp& prop, std::uint32_t offset)
{
	UTIL_DEBUG_ASSERT(client_class_name);

	#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
	*dumpFile
		<< "[NETVAR] Address: 0x"
		<< std::hex << &prop << std::dec
		<< " Name: \""
		<< prop.name
		<< "\" Offset: "
		<< offset + prop.offset
		<< " Type: "
		<< int(prop.type)
		<< '\n';
	#endif

	switch (prop.type)
	{
		case RecvProp::Type::Int:
		case RecvProp::Type::Float:
		case RecvProp::Type::String:
		case RecvProp::Type::Vector:
		case RecvProp::Type::NumSendPropTypes:
			_offsets.insert(
				std::make_pair(
					UTIL_RUNTIME_HASH(UTIL_FORMAT(
						client_class_name << ':' << prop.name
					)),
					offset + prop.offset
				)
			);
			break;

		case RecvProp::Type::Array:
			#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
			*dumpFile << "[BEGIN PROP TREE]\n";
			#endif

			if (prop.propArray)
				for (auto i = 0; i < prop.elementCount; i++)
					Parse(client_class_name, prop.propArray[i], offset + prop.offset);
			#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
			else
				*dumpFile << "!!! GOT NULL PROP TREE !!!\n";
			#endif

			#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
			*dumpFile << "[END PROP TREE]\n";
			#endif

			break;

		case RecvProp::Type::DataTable:
			if (prop.table)
				Parse(client_class_name, *prop.table, offset + prop.offset);
			#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
			else
				*dumpFile << "!!! TRIED TO PARSE DATATABLE BUT TABLE IS NULL !!!\n";
			#endif

			break;

			#if BUILD_GAME_IS_GMOD
		case RecvProp::Type::GmodDataTable:
			break;
			#endif

		default:
			// skipped switch element
			UTIL_DEBUG_ASSERT(false);
			break;
	}
}

SourceSDK::NetvarManager::NetvarManager()
{
#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
	UTIL_CHECK_ALLOC(dumpFile = new std::ofstream(SOURCE_SDK_NETVARS_MGR_DUMP));

	UTIL_DEBUG_ASSERT(dumpFile->good());
#endif

	for (auto cclass = interfaces->clientDLL->GetHeadClientClass(); cclass; cclass = cclass->next)
	{
		#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
		*dumpFile
			<< "[CLASSID] Address: 0x"
			<< std::hex << cclass << std::dec
			<< " Name: \""
			<< cclass->networkName
			<< "\" ID:"
			<< int(cclass->id)
			<< '\n';
		#endif

		Parse(cclass->networkName, cclass->recvTable);
	}

	#ifdef SOURCE_SDK_NETVARS_MGR_DUMP
	dumpFile->close();
	delete dumpFile;
	#endif
}

std::size_t SourceSDK::NetvarManager::FindOffset(Util::HashType net_name)
{
	UTIL_LABEL_ENTRY(UTIL_SXOR(L"Find Netvar Offset ") + std::to_wstring(net_name));

	if (auto iter = this->_offsets.find(net_name); iter != this->_offsets.end())
	{
		UTIL_LABEL_OK();
		return (*iter).second;
	}
	else
		UTIL_LABEL_FAIL();
}
