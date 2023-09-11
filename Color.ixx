export module Color;

import <iostream>;

import Vec3;
import Interval;

float LinearToGammaSpace(float linearColorComponent)
{
	return sqrtf(linearColorComponent);
}

export void PrintAsColor(std::ostream& ostream, 
	const Vec3& v, int samplesPerPixel)
{
	static const Interval intensity(0.0f, 0.999f);

	const float scale = 1.0f / samplesPerPixel;

	const float r = LinearToGammaSpace(v.R() * scale);
	const float g = LinearToGammaSpace(v.G() * scale);
	const float b = LinearToGammaSpace(v.B() * scale);

	ostream << static_cast<int>(256.0f * intensity.Clamp(r)) << ' '
			<< static_cast<int>(256.0f * intensity.Clamp(g)) << ' '
			<< static_cast<int>(256.0f * intensity.Clamp(b)) << "\n";
}
