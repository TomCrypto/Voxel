#pragma once

#include <vector>
#include <cassert>

namespace Voxel
{
	struct Pixel
	{
		float r, g, b, a;
	};

	// A screen raster, which holds a pixel buffer
	struct Raster
	{
		private:
			std::vector<Pixel> m_data;
			size_t width, height;

		public:
			Raster(size_t width, size_t height) : width(width), height(height)
			{
				assert((width > 0) && (height > 0));
				m_data.reserve(width * height);
			}
			
			const Pixel& operator()(const size_t x, const size_t y)
			{
				assert((x < width) && (y < height));
				return m_data[y * width + x];
			}
	};

	class Display
	{
		public:
			virtual ~Display() { }
			virtual void Present(Raster raster);
	};
};