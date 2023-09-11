#define OUT

export module PPMUtility;

import <string>;
import <fstream>;
import <iostream>;

import Vec3;
import Color;

namespace PPMUtil
{
	bool TryCreateFile(const std::string& fileNameWithoutExtension, OUT std::ofstream& fileStream);
	void PrintHeader(std::ofstream& outf, int imgWidth, int imgHeight);
	void PrintColors(std::ofstream& outf, int imgWidth, int imgHeight, 
		Vec3 const* rgbTriplets, int samplesPerPixel);

	export bool TrySaveAsPPM(
		int imgWidth,
		int imgHeight,
		Vec3 const* rgbTriplets,
		int samplesPerPixel,
		const std::string& fileNameWithoutExtension)
	{
		std::ofstream outf;
		if (!TryCreateFile(fileNameWithoutExtension, OUT outf))
		{
			return false;
		}

		PrintHeader(outf, imgWidth, imgHeight);
		PrintColors(outf, imgWidth, imgHeight, rgbTriplets,
					samplesPerPixel);

		return true;
	}

	void PrintColors(std::ofstream& outf, 
		int imgWidth,
		int imgHeight,
		Vec3 const* rgbTriplets,
		int samplesPerPixel)
	{
		std::size_t index = 0;

		for (int j = 0; j < imgHeight; ++j)
		{
			for (int i = 0; i < imgWidth; ++i)
			{
				PrintAsColor(outf, rgbTriplets[index++], samplesPerPixel);
			}
		}
	}

	void PrintHeader(std::ofstream& outf, int imgWidth, int imgHeight)
	{
		outf << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";
	}

	bool TryCreateFile(const std::string& fileNameWithoutExtension,
		OUT std::ofstream& fileStream)
	{
		const std::string fileName { fileNameWithoutExtension + ".ppm" };
		fileStream = std::ofstream{ fileName };

		return fileStream ? true : false;
	}
} // namespace PPMUtil