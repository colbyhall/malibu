#pragma once

#include "../types.hpp"

namespace core::math {
	constexpr f32 PI = 3.14159265359f;
	constexpr f32 TAU = PI * 2.0;
	constexpr f32 DEG_TO_RAD = PI / 180.f;
	constexpr f32 RAD_TO_DEG = 180.f / PI;
	constexpr f32 NEAR_ZERO = 1.0e-8f;

	f32 cos(f32 x);
	f64 cos(f64 x);
	
	f32 sin(f32 x);
	f64 sin(f64 x);
	
	f32 tan(f32 x);
	f64 tan(f64 x);
	
	f32 acos(f32 x);
	f64 acos(f64 x);
	
	f32 asin(f32 x);
	f64 asin(f64 x);
	
	f32 atan(f32 x);
	f64 atan(f64 x);
	
	f32 atan2(f32 y, f32 x);
	f64 atan2(f64 y, f64 x);
	
	f32 sqrt(f32 x);
	f64 sqrt(f64 x);

	f32 fmod(f32 numerator, f32 denominator);
	f64 fmod(f64 numerator, f64 denominator);
}