#pragma once
#include "../valvesdk/interfaces.hpp"

#include "matrix.hpp"
#include "vector.hpp"
#include "overridetype.hpp"

namespace SourceSDK
{
	struct Model;
	struct StudioHDR;

	class Renderable;
	class Material;


	using ModelInstanceHandle = std::uint16_t;
	using StudioDecalHandle = void*;

	struct DrawModelState final
	{
		StudioHDR* studioHdr;
		void** studioHWData;
		Renderable* renderable;
		const Matrix3x4* modelToWorld;
		StudioDecalHandle decals;
		int drawFlags;
		int lod;
	};

	struct ModelRenderInfo final
	{
		Vector3 origin;
		Angle angles;
		Renderable* renderable;
		const Model* model;
		const Matrix3x4* modelToWorld;
		const Matrix3x4* lightingOffset;
		int flags, entityIndex, skin, body, hitboxSet;
		ModelInstanceHandle instance;
	};

	class ModelRender final
	{
	public:
		std::size_t GetDrawModelExecuteIndex();

		void DrawModelExecute(const SourceSDK::DrawModelState& state,
							  const SourceSDK::ModelRenderInfo& info,
							  SourceSDK::Matrix3x4* boneToWorld);

		void ForcedMaterialOverride(Material* material, OverrideType type = OverrideType::Normal);
	};

	VALVE_SDK_INTERFACE_DECL(ModelRender, modelrender);
}
