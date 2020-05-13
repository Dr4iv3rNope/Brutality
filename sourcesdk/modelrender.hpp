#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../build.hpp"

#include "matrix.hpp"
#include "vector.hpp"
#include "overridetype.hpp"

#include "../util/pad.hpp"

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
		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(0, 12);
		int entityIndex, skin, body, hitboxSet;
		#else
		const Matrix3x4* lightingOffset;
		int flags, entityIndex, skin, body, hitboxSet;
		#endif
		ModelInstanceHandle instance;
	};

	struct DrawModelExecuteArgs final
	{
		const SourceSDK::DrawModelState* state;
		const SourceSDK::ModelRenderInfo* info;
		SourceSDK::Matrix3x4* boneToWorld;

		inline DrawModelExecuteArgs(const SourceSDK::DrawModelState& state,
									const SourceSDK::ModelRenderInfo& info,
									SourceSDK::Matrix3x4* boneToWorld) noexcept
			: state { &state }, info { &info }, boneToWorld { boneToWorld }
		{}
	};

	class ModelRender final
	{
	public:
		std::size_t GetDrawModelExecuteIndex();

		void ForcedMaterialOverride(Material* material, OverrideType type = OverrideType::Normal);
	};
}
