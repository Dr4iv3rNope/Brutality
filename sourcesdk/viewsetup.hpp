#pragma once
#include "sdk.hpp"
#include "vector.hpp"

namespace SourceSDK
{
	struct ViewSetup final
	{
		int x, y, w, h;

		struct
		{
			bool enabled;

			float left, top, right, bottom;
		} ortho;

		float fov;
		float fov_viewmodel;

		Vector origin;
		Angle angles;

		float z_near;
		float z_far;
		float z_near_viewmodel;
		float z_far_viewmodel;

		bool render_to_subrect_of_larger_screen;
		float aspect_ratio;

		struct
		{
			bool enabled;

			float top, bottom, left, right;
		} off_center;

		bool bloom_and_tone_mapping;
		bool cache_full_scene_state;
	};
}
