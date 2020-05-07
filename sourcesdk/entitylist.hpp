#pragma once
#include "interfaces.hpp"
#include "entity.hpp"

namespace SourceSDK
{
	class Networkable;

	class ClientEntityList final
	{
	public:
		// get entity by entity idx
		BaseEntity* GetEntity(int idx);

		// get networkable from handle
		Networkable* GetNetworkable(EntityHandle handle);

		int GetMaxEntities();
	};

	SOURCE_SDK_INTERFACE_DECL(ClientEntityList, entitylist);
}
