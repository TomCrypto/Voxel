#include "raster.hpp"

#include <X11/Xlib.h>
#include <GL/glx.h>

class X11Display
{
public:
	X11Display(const char *params) { init(params); }
	~X11Display();

	// @summary: Attempts to present the raster object visually on the display.
	// @returns: true upon success, otherwise false.
	bool draw(const Raster &);

private:
	
	int screen;
	XVisualInfo *vi;
	Window window;
	Display *server;
	GLXContext gl;
	Atom wm_del;

	void init(const char *params);
};
