#pragma once
#include "logs.hpp"

#ifdef __UTIL_DISABLE_LOGS
#define __UTIL_DISABLE_LABELS
#endif

#ifndef __UTIL_DISABLE_LABELS
#include "assert.hpp"
#include <memory>

namespace Util
{
	namespace Debug
	{
		class LogLabel
		{
		private:
			std::wstring _what;
			
			enum
			{
				LLStatus_Success,
				LLStatus_Failed,	// will assert
				LLStatus_Overrided, // will not assert, but will say that label failed
				LLStatus_Forbidden	// dev error: label has been released them self
			} _status{ LLStatus_Forbidden };

		public:
			inline LogLabel(const std::wstring& what) noexcept
			{
				this->_what = what;

				UTIL_LOG(UTIL_SXOR(L"[LogLabel] Entry -~=> ") + what);
			}

			inline ~LogLabel()
			{
				switch (this->_status)
				{
					case LLStatus_Success:
						UTIL_LOG(UTIL_SXOR(L"[LogLabel] Exit <=~- ") + this->_what);
						break;

					case LLStatus_Failed:
						UTIL_LOG(UTIL_SXOR(L"[LogLabel] Failed -~x~- ") + this->_what);
						UTIL_ASSERT(false, "Log label failed");
						break; // unreachable

					case LLStatus_Overrided:
						UTIL_LOG(UTIL_SXOR(L"[LogLabel] Override Fail <=~?.. ") + this->_what);
						break;

					case LLStatus_Forbidden:
						UTIL_LOG(UTIL_SXOR(L"[LogLabel] Forbidden =>*** ") + this->_what);
						UTIL_ASSERT(false, "Log label is forbidden");
						break; // unreachable
				}
			}

			inline const std::wstring& What() noexcept
			{
				return this->_what;
			}

			static inline void ExitSuccess(LogLabel* label)
			{
				label->_status = LLStatus_Success;
				delete label;
			}

			static inline void ExitFailure(LogLabel* label)
			{
				label->_status = LLStatus_Failed;
				delete label;
			}

			static inline void ExitOverride(LogLabel* label)
			{
				label->_status = LLStatus_Overrided;
				delete label;
			}
		};
	}
}

#define UTIL_LABEL_EX(id)	__log_label_##id	// returns label name by id
#define UTIL_LABEL()		UTIL_LABEL_EX(_)	// returns default label name

#define UTIL_LABEL_ENTRY_EX(id, what) \
	auto UTIL_LABEL_EX(id) = std::unique_ptr<Util::Debug::LogLabel>(new Util::Debug::LogLabel(what))	// creates new label entry by id
#define UTIL_LABEL_ENTRY(what)	UTIL_LABEL_ENTRY_EX(_, what)											// creates new label with default name

#define UTIL_LABEL_FAIL_EX(id)	Util::Debug::LogLabel::ExitFailure(UTIL_LABEL_EX(id).release())	// fails unique label
#define UTIL_LABEL_FAIL()		UTIL_LABEL_FAIL_EX(_)											// fails default label

#define UTIL_LABEL_OK_EX(id)	Util::Debug::LogLabel::ExitSuccess(UTIL_LABEL_EX(id).release())	// succeeds unique label
#define UTIL_LABEL_OK()			UTIL_LABEL_OK_EX(_)												// succeeds default label

#define UTIL_LABEL_OVERRIDE_EX(id)	Util::Debug::LogLabel::ExitOverride(UTIL_LABEL_EX(id).release())	// disables assert and fail unique label
#define UTIL_LABEL_OVERRIDE()		UTIL_LABEL_OVERRIDE_EX(_)											// disables assert and fail default label
#else
#define UTIL_LABEL_EX(id)
#define UTIL_LABEL()

#define UTIL_LABEL_ENTRY_EX(id, what)
#define UTIL_LABEL_ENTRY(what)

#define UTIL_LABEL_FAIL_EX(id)
#define UTIL_LABEL_FAIL()

#define UTIL_LABEL_OK_EX(id)
#define UTIL_LABEL_OK()

#define UTIL_LABEL_OVERRIDE_EX(id)
#define UTIL_LABEL_OVERRIDE()
#endif // __UTIL_DISABLE_LOGS
