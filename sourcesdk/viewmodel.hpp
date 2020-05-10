#pragma once
#include "entity.hpp"

namespace SourceSDK
{
	class ViewModel : public BaseEntity
	{
	public:
		SOURCE_SDK_NETVAR(int, GetSkin, "CBaseViewModel", "m_nSkin", 0);
		SOURCE_SDK_NETVAR(int, GetBody, "CBaseViewModel", "m_nBody", 0);
		SOURCE_SDK_NETVAR(int, GetSequence, "CBaseViewModel", "m_nSequence", 0);
		SOURCE_SDK_NETVAR(int, GetViewModelIndex, "CBaseViewModel", "m_nViewModelIndex", 0);
		SOURCE_SDK_NETVAR(float, GetPlaybackRate, "CBaseViewModel", "m_flPlaybackRate", 0);
		SOURCE_SDK_NETVAR(int, GetAnimationParity, "CBaseViewModel", "m_nAnimationParity", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetWeapon, "CBaseViewModel", "m_hWeapon", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetOwner, "CBaseViewModel", "m_hOwner", 0);
		SOURCE_SDK_NETVAR(int, GetNewSequenceParity, "CBaseViewModel", "m_nNewSequenceParity", 0);
		SOURCE_SDK_NETVAR(int, GetResetEventsParity, "CBaseViewModel", "m_nResetEventsParity", 0);
		SOURCE_SDK_NETVAR(int, GetMuzzleFlashParity, "CBaseViewModel", "m_nMuzzleFlashParity", 0);
	};
}
