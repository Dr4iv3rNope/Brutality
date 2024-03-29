#pragma once
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
}
