#include <cstdlib>

#ifdef TESTING

#include "testing/testing.h"

int main(int argc, char* argv[])
{
    Testing::RunTests();
    return EXIT_SUCCESS;
}

#else

// main program goes here

#include "renderer.h"

#include <Windows.h>
#include <fstream>

int main(int argc, char* argv[])
{
    Renderer renderer(1024, 768);

	const Pixel* render = renderer.Render();

	std::fstream file("render.bmp", std::ios_base::out);

	BITMAPFILEHEADER tWBFH;
	tWBFH.bfType = 0x4d42;
	tWBFH.bfSize = 14 + 40 + (renderer.width * renderer.height * 3);
	tWBFH.bfReserved1 = 0;
	tWBFH.bfReserved2 = 0;
	tWBFH.bfOffBits = 14 + 40;

	BITMAPINFOHEADER tW2BH;
	memset(&tW2BH,0,40);
	tW2BH.biSize = 40;
	tW2BH.biWidth = renderer.width;
	tW2BH.biHeight = renderer.height;
	tW2BH.biPlanes = 1;
	tW2BH.biBitCount = 24;
	tW2BH.biCompression = 0;

	file.write((char*)(&tWBFH),14);
	file.write((char*)(&tW2BH),40);

	for (int y = renderer.height - 1; y >= 0; --y)
		for (int x = 0; x < renderer.width; ++x)
		{
			byte r = (byte)(render->r * 255);
			byte g = (byte)(render->g * 255);
			byte b = (byte)(render->b * 255);

			file.write((const char*)&b, 1); // BGRA format
			file.write((const char*)&g, 1);
			file.write((const char*)&r, 1);

			++render;
		}

	file.close();

	return EXIT_SUCCESS;
}

#endif
