#pragma once
#include "xorstr.hpp"

#include <string>
#include <list>
#include <utility>

namespace Util
{
	namespace HTTP
	{
		struct Response final
		{
			int statusCode;
			std::string content;
		};

		using Header = std::pair<std::wstring, std::wstring>;
		using HeaderList = std::list<Header>;

		// can throw exceptions
		extern void Request(
			const std::wstring& url,
			const std::wstring& request_type,	// "GET", "POST", etc..
			const std::string& content = std::string(),
			const HeaderList& headers = HeaderList(),
			bool ssl = true,					// use SSL connection?
			Response* response = nullptr,		// if it's not, then we'll wait for response
			const std::wstring& user_agent = UTIL_SXOR(L"Brutality HTTP")
		);

		// for argument description, go check Util::HTTP::Request
		//
		// the difference between Request and RequestNoThrow,
		// the second function doesn't throw any exceptions
		inline bool RequestNoThrow(
			const std::wstring& url,
			const std::wstring& request_type,
			const std::string& content = std::string(),
			const HeaderList& headers = HeaderList(),
			bool ssl = true,
			Response* response = nullptr,
			const std::wstring& user_agent = UTIL_SXOR(L"Brutality HTTP")
		) noexcept
		{
			try
			{
				Request(url, request_type, content, headers, ssl, response, user_agent);
				
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
	}
}
