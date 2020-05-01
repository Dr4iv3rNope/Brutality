#pragma once

namespace SourceSDK
{
	enum class ClientFrameStage : int
	{
		Undefined = -1,
		Start,

		NetUpdateStart,
		NetUpdatePostDataUpdateStart,
		NetUpdatePostDataUpdateEnd,
		NetUpdateEnd,

		RenderStart,
		RenderEnd
	};
}
