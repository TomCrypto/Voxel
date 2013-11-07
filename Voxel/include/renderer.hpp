#include <cstdlib>

struct Pixel
{
	float r, g, b, a;
};

class Renderer
{
	private:
		Pixel* pixels;

	public:
		Renderer(size_t width, size_t height);
		~Renderer();

		size_t width, height;

		const Pixel* Render();
};
