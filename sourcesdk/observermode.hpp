#pragma once

namespace SourceSDK
{
	// game/shared/shareddefs.h:377
	enum class ObserverMode
	{
		None,
		DeathCam,
		FreezeCam,
		Fixed,		// view from a fixed camera position
		InEye,		// first person
		Chase,		// third person
		Roaming
	};
}
