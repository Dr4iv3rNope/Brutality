#include "httpclient.hpp"
#include "strings.hpp"
#include "stringhash.hpp"

#include "debug/winapi.hpp"
#include "debug/errors.hpp"
#include "debug/labels.hpp"

#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

void Util::HTTP::Request(
	const std::wstring& url,
	const std::wstring& request_type,
	const std::string& content,
	const HeaderList& headers,
	bool ssl,
	Response* response,
	const std::wstring& user_agent
)
{
	UTIL_LABEL_ENTRY(UTIL_WFORMAT(
		UTIL_XOR(L"Http Request ") <<
		UTIL_RUNTIME_HASH(url) << ' ' <<
		UTIL_RUNTIME_HASH(request_type)
	));

	HINTERNET session = WinHttpOpen(user_agent.c_str(), WINHTTP_ACCESS_TYPE_NO_PROXY, nullptr, nullptr, 0);

	if (!session)
	{
		Util::Debug::LogLastWinapiError();

		UTIL_LABEL_OVERRIDE();
		UTIL_RUNTIME_ERROR("Failed to create http session");
	}

	HINTERNET connection = WinHttpConnect(
		session,
		url.c_str(),
		ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
		0
	);

	if (!connection)
	{
		Util::Debug::LogLastWinapiError();

		WinHttpCloseHandle(session);

		UTIL_LABEL_OVERRIDE();
		UTIL_RUNTIME_ERROR("Failed to create http connection");
	}

	HINTERNET request = WinHttpOpenRequest(
		connection,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		ssl ? WINHTTP_FLAG_SECURE : 0
	);

	if (!request)
	{
		Util::Debug::LogLastWinapiError();

		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);

		UTIL_LABEL_OVERRIDE();
		UTIL_RUNTIME_ERROR("Failed to open http request");
	}

	std::wstring str_headers;
	{
		for (const auto& header : headers)
			str_headers += UTIL_WFORMAT(
				header.first << UTIL_XOR(L": ") <<
				header.second << UTIL_XOR(L"\r\n")
			);
	}

	if (!WinHttpSendRequest(
		request,
		str_headers.c_str(),
		-1,
		LPVOID(content.c_str()),
		content.size(),
		content.size(),
		0
	))
	{
		Util::Debug::LogLastWinapiError();

		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);

		UTIL_LABEL_OVERRIDE();
		UTIL_RUNTIME_ERROR("Failed to send http request");
	}

	if (response)
	{
		if (!WinHttpReceiveResponse(request, nullptr))
		{
			Util::Debug::LogLastWinapiError();

			WinHttpCloseHandle(request);
			WinHttpCloseHandle(connection);
			WinHttpCloseHandle(session);

			UTIL_LABEL_OVERRIDE();
			UTIL_RUNTIME_ERROR("Failed to receive http response");
		}

		std::string content;
		DWORD available = 0;

		do
		{
			if (!WinHttpQueryDataAvailable(request, &available))
			{
				Util::Debug::LogLastWinapiError();

				WinHttpCloseHandle(request);
				WinHttpCloseHandle(connection);
				WinHttpCloseHandle(session);

				UTIL_LABEL_OVERRIDE();
				UTIL_RUNTIME_ERROR("Failed to query available data size in http response");
			}

			if (!available)
				break;

			{
				char buffer[1 << 8];
				DWORD readed;

				if (!WinHttpReadData(request, buffer, sizeof(buffer), &readed))
				{
					Util::Debug::LogLastWinapiError();

					WinHttpCloseHandle(request);
					WinHttpCloseHandle(connection);
					WinHttpCloseHandle(session);

					UTIL_LABEL_OVERRIDE();
					UTIL_RUNTIME_ERROR("Failed to read data from http response");
				}
			}
		}
		while (available > 0);

		response->content = content;

		if (DWORD statusCode, size; WinHttpQueryHeaders(
			request,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&statusCode,
			&size,
			nullptr
		))
		{
			response->statusCode = statusCode;
		}
		else
		{
			Util::Debug::LogLastWinapiError();
			UTIL_XLOG(L"Failed to query http status code");

			response->statusCode = -1;
		}
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);
	WinHttpCloseHandle(session);
	
	UTIL_LABEL_OK();
}
