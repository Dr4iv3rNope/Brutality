#pragma once
#include "interfaces.hpp"
#include "entity.hpp"

namespace SourceSDK
{
	class ClientEntityList final
	{
	public:
		// get entity by entity idx
		BaseEntity* GetEntity(int idx);

		// get entity from handle
		BaseEntity* GetEntity(EntityHandle handle);

		int GetMaxEntities();
	};

	SOURCE_SDK_INTERFACE_DECL(ClientEntityList, entitylist);
}
