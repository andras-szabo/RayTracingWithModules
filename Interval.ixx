export module Interval;

import Utility;

export struct Interval
{
			Interval() = default;
			Interval(float min_, float max_) :
				min{ min_ }, max{ max_ } {}

	inline bool		DoesContain(float x) const;
	inline bool		DoesSurround(float x) const;
	float			Clamp(float x) const;

	float			min{ infinity };
	float			max{ -infinity };
};

float Interval::Clamp(float x) const
{
	if (x < min)
	{
		return min;
	}

	if (x > max)
	{
		return max;
	}

	return x;
}

bool inline Interval::DoesContain(float x) const
{
	return min <= x && x <= max;
}

bool inline Interval::DoesSurround(float x) const
{
	return min < x && x < max;
}