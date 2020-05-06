#include "clientclassdumper.hpp"
#include "hlclient.hpp"

#include "../util/debug/errors.hpp"

#include "../util/strings.hpp"

#include <stdexcept>

SourceSDK::ClientClassDumper::ClientClassDumper()
{
	for (auto cclass = clientDLL->GetHeadClientClass(); cclass; cclass = cclass->next)
		this->_clientclasses.insert(std::make_pair(cclass->networkName, cclass));
}

const SourceSDK::ClientClass* SourceSDK::ClientClassDumper::FindClientClass(const std::string& cs_name)
{
	if (auto iter = this->_clientclasses.find(cs_name); iter != this->_clientclasses.end())
		return (*iter).second;
	else
		UTIL_RUNTIME_ERROR_EX(UTIL_WFORMAT(
			UTIL_XOR(L"failed to find client class ") <<
			Util::ToWideChar(cs_name)
		), UTIL_XOR("failed to find client class"));
}
