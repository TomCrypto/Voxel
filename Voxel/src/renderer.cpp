#include "renderer.hpp"

Renderer::Renderer(size_t width, size_t height) : width(width), height(height)
{
	pixels = new Pixel[width * height];
}

Renderer::~Renderer()
{
	delete[] pixels;
}

const Pixel* Renderer::Render()
{
	for (size_t y = 0; y < height; ++y)
		for (size_t x = 0; x < width; ++x)
		{
			pixels[y * width + x].r = 0.75f;
			pixels[y * width + x].g = 0.25f;
			pixels[y * width + x].b = 0.25f;
		}

	return pixels;
}
