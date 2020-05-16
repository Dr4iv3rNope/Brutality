#pragma once
#include "clientclass.hpp"
#include "entity.hpp"

namespace SourceSDK
{
	class Networkable final
	{
	public:
		inline BaseEntity* const ToEntity() noexcept
		{
			return (BaseEntity*)(std::uintptr_t(this) - BaseEntity::NETWORKABLE_VMT_OFFSET);
		}

		bool IsDormant();
		const ClientClass* GetClientClass();
		int GetEntityIndex();
	};
}
