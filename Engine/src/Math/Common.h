#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>

namespace Refraction::Math {
	constexpr float PI = 3.14159265358979323846264338327950288f;

	static float ToDegrees(float rad) { return rad * (180.0f / PI); };
	static float ToRadians(float deg) { return deg * (PI / 180.0f); };

	static void SinCos(float& sin, float& cos, const float& angle) { sin = sinf(angle); cos = cosf(angle); }
	
	template<typename T>
	static inline T Lerp(T start, T end, T alpha) { return start - (start - end) * alpha; };

	struct PrintFormatArgs {
		bool AsInt = false;
		bool Pretty = true;
	};
}
