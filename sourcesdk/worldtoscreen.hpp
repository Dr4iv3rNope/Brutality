#pragma once
#include "vector.hpp"

namespace SourceSDK
{
	// WorldToScreen function output
	struct W2SOutput
	{
		Vector2 screen;

		// if this is false then
		// screen will be always 0; 0
		bool isBehindCamera;

		inline W2SOutput() noexcept : screen {}, isBehindCamera { true } {}
		inline W2SOutput(const Vector2& screen) noexcept
			: screen { screen }, isBehindCamera { false } {}
		
		inline W2SOutput(const Vector& screen) noexcept
			: W2SOutput(Vector2(screen.x, screen.y)) {}

		// return true if in front of the camera
		inline explicit operator bool() const noexcept { return !isBehindCamera; }

		inline explicit operator Vector2() const noexcept { return screen; }
		inline const Vector2* operator->() const noexcept { return &screen; }
	};

	W2SOutput WorldToScreen(const Vector& world);
}
