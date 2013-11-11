#include "display/x11_display.hpp"

#include <X11/Xosdefs.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <GL/glx.h>

#include <stdexcept>


void X11Display::init(const char *params)
{
	int pixel_descriptor[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

	// create x context
	server = XOpenDisplay(NULL);
	if (server == NULL)
		throw std::runtime_error("couldnt connect to X server");
	screen = DefaultScreen(server);

	// get the closest to optimum pixel settings
	vi = glXChooseVisual(server, screen, pixel_descriptor);

	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(server, RootWindow(server, vi->screen), vi->visual, AllocNone);
	attr.border_pixel = 0;
	attr.event_mask = ExposureMask /*| KeyPressMask | KeyReleaseMask
	  | ButtonPressMask | PointerMotionMask*/ | StructureNotifyMask | ClientMessage;

	// create window
	window = XCreateWindow(server, RootWindow(server, screen), 0, 0, 800, 600, 0,
				           vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask,
			               &attr);
	if (window == 0)
		throw std::runtime_error("failed to create window");

	// set the window's title
	XStoreName(server, window, "moo");

	// show window
	XMapRaised(server, window);

	// create opengl context
	gl = glXCreateContext(server, vi, NULL, GL_TRUE);
	if (gl == 0)
		throw std::runtime_error("failed to create OpenGL context");
	
	// switch to opengl context
	if (!glXMakeCurrent(server, window, gl))
		throw std::runtime_error("failed to switch OpenGL context");

	// carry out the above tasks
	XFlush(server);

	// listen for window manager's close event
	wm_del = XInternAtom(server, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(server, window, &wm_del, 1);

	// make sure input is going to window
	XSetInputFocus(server, window, None, CurrentTime);
}

bool X11Display::draw(const Raster &raster)
{
	XEvent event;
	while (XPending(server) > 0) {
		XNextEvent(server, &event);
		switch (event.type) {
			// skip to redraw if part exposed
			case Expose: break;
			// the window was resized
			case ConfigureNotify: {
				break;
			}
			// message from window manager
			case ClientMessage: {
				// the close button on the window was pushed
				if( event.xclient.data.l[0] == wm_del )
					return false;
				break;
			}
		}
	}

	glXSwapBuffers(server, window);
	XFlush(server);
	return true;
}

X11Display::~X11Display()
{
	if (server == nullptr) return;
	if (window != 0) {
		if (gl != nullptr) {

			// switch the current opengl context off
			glXMakeCurrent(server, None, NULL );

			// destroy our opengl context
			glXDestroyContext(server, gl);
			gl = nullptr;
		}

		if (vi != nullptr) {
			XFree(vi);
			vi = 0;
		}

		// destroy the window
		XDestroyWindow(server, window);
		window = 0;

		// destroy the connection with the x server
		XCloseDisplay(server);
		server = NULL;
	}
}
