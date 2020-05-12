#pragma once
#include "../build.hpp"

#if BUILD_GAME_IS_GMOD
#include "../../sourcesdk/entity.hpp"

namespace SourceSDK
{
	class GmodEntity : public BaseEntity
	{
	public:
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_ubInterpolationFrame", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "moveparent", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_Collision", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_takedamage", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_RealClassName", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_OverrideMaterial", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_OverrideSubMaterials[0]", 0);
		SOURCE_SDK_NETVAR(bool, IsOnFire, "CBaseEntity", "m_bOnFire", 0);
		//SOURCE_SDK_NETVAR(, , "CBaseEntity", "m_CreationTime", 0);
	};
}

#endif
