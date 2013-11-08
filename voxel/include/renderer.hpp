#include <cstdlib>

/* Indicative interface for the Renderer */

class Renderer
{
	private:
	    // some state, and at least a memory buffer to hold the render

	public:
        // DisplayInfo contains stuff like render width, height, etc..
		Renderer(DisplayInfo info,
                 Projection projection,
                 Integrator integrator);

        // Renders the frame, and returns a raster of pixels (TBD)
		const DisplayRaster render();
};
