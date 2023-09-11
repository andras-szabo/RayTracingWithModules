export module Utility;

import <limits>;
import <cstdlib>;
import <array>;

export const float infinity = std::numeric_limits<float>::infinity();
export const float pi = 3.14159265f;
export inline float ToRadians(float degrees)
{
	return degrees * pi / 180.0f;
}

std::array<float, 256>		randomNumbers;
std::size_t					randomNumberIndex { 0 };
bool						didCacheRandomNumbers{ false };


export float GetRandomFloat()
{
	if (didCacheRandomNumbers == false)
	{
		for (int i = 0; i < 256; ++i)
		{
			randomNumbers[i] = rand() / (RAND_MAX + 1.0f);
		}

		randomNumberIndex = 0;
		didCacheRandomNumbers = true;
	}

	const float rnd = randomNumbers[randomNumberIndex];
	randomNumberIndex = (randomNumberIndex + 1) % 256;
	return rnd;
}

export inline float GetRandomFloat(float min, float max)
{
	return min + (max - min) * GetRandomFloat();
}
