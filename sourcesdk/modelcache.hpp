#pragma once
#include "interfaces.hpp"
#include "model.hpp"
#include "studio.hpp"

namespace SourceSDK
{
	class MDLCache final
	{
	public:
		const StudioHDR* GetStudioHDR(MDLHandle handle);

		// return nullptr if @model is nullptr or
		// @model type is not Model::Type::Studio
		inline const StudioHDR* GetStudioHDR(const Model* model) noexcept
		{
			return model && model->type == Model::Type::Studio
				? this->GetStudioHDR(model->studioHandle)
				: nullptr;
		}
	};

	SOURCE_SDK_INTERFACE_DECL(MDLCache, mdlCache);
}
