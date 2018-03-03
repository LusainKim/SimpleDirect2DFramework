#pragma once

#include <cmath>
#include <algorithm>

struct Color 
{ 
	union
	{
		// rgb : [0, 1]
		struct { float r, g, b; };
		// h : [0, 360], sv : [0, 1]
		struct { float h, s, v; };
	};
	// alpha value
	float a;

	Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f) noexcept
		: r { r }
		, g { g }
		, b { b }
		, a { a }
	{
	}

	static Color HSVtoRGB(Color hsv)
	{
		Color rgb;

		// achromatic (grey)
		if (hsv.s == 0.f) return Color{ hsv.v, hsv.v, hsv.v };

		// sector 0 to 5
		hsv.h /= 60.f;

		auto i = static_cast<int>(std::floor(hsv.h));

		// factorial part of h
		auto f = hsv.h - i;          
		auto p = hsv.v * (1.f - hsv.s);
		auto q = hsv.v * (1.f - hsv.s * f);
		auto t = hsv.v * (1.f - hsv.s * (1.f - f));

		switch (i) 
		{
		case 0:		return Color{ hsv.v, t, p };
		case 1:		return Color{ q, hsv.v, p };
		case 2:		return Color{ p, hsv.v, t };
		case 3:		return Color{ p, q, hsv.v };
		case 4:		return Color{ t, p, hsv.v };
		case 5:		return Color{ hsv.v, p, q };
		default:	return Color{};
		}
	}

	// r,g,b values are from 0 to 1
	// h = [0,360], s = [0,1], v = [0,1]
	//		if s == 0, then h = -1 (undefined)
	static Color RGBtoHSV(Color rgb)
	{
		Color hsv;

		float _min = std::min(std::min(rgb.r, rgb.g), rgb.b);
		float _max = std::max(std::max(rgb.r, rgb.g), rgb.b);
		float delta = _max - _min;

		// between yellow & magenta
		if (rgb.r == _max)			hsv.h = 0.f + (rgb.g - rgb.b) / delta;
		// between cyan & yellow
		else if (rgb.g == _max)		hsv.h = 2.f + (rgb.b - rgb.r) / delta;
		// between magenta & cyan
		else						hsv.h = 4.f + (rgb.r - rgb.g) / delta;	

		// degrees
		hsv.h *= 60.f;				
	
		if (hsv.h < 0) hsv.h += 360;

		// s
		if (_max != 0) hsv.s = delta / _max;		
		else 
		{
			// r = g = b = 0		// s = 0, v is undefined
			hsv.s = 0;
			hsv.h = -1.f;
			return hsv;
		}
	
		// v
		hsv.v = _max;	

		return hsv;
	}
};
