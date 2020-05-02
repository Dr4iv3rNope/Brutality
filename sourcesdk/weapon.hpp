#pragma once
#include "entity.hpp"
#include "weaponstate.hpp"
#include "fileweaponinfo.hpp"

namespace SourceSDK
{
	class BaseWeapon : public BaseEntity
	{
	public:
		SOURCE_SDK_NETVAR(int, GetViewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex", 0);
		SOURCE_SDK_NETVAR(int, GetWorldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex", 0);
		SOURCE_SDK_NETVAR(WeaponState, GetWeaponState, "CBaseCombatWeapon", "m_iState", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetOwner, "CBaseCombatWeapon", "m_hOwner", 0);

		const FileWeaponInfo& GetWeaponInfo();
	};
}
