#include "display/x11_display.hpp"

#include <X11/Xosdefs.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <GL/glx.h>
#include <GL/glu.h>

#include <stdexcept>
#include <limits>

#ifdef DEBUG
#include <iostream> // TEMP
#endif

void X11Display::init(const char *name)
{
	int pixel_descriptor[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

	// Create X server connection.
	server = XOpenDisplay(NULL);
	if (server == NULL)
		throw std::runtime_error("couldnt connect to X server");
	screen = DefaultScreen(server);

	// Get the closest to optimum pixel settings.
	vi = glXChooseVisual(server, screen, pixel_descriptor);

	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(server, RootWindow(server, vi->screen), vi->visual, AllocNone);
	attr.border_pixel = 0;
	attr.event_mask = StructureNotifyMask | ClientMessage;

	// Create window
	window = XCreateWindow(server, RootWindow(server, screen), 0, 0, width, height, 0,
				           vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask,
			               &attr);
	if (window == 0)
		throw std::runtime_error("failed to create window");

	// Set the window's title.
	XStoreName(server, window, name);

	// Show window.
	XMapRaised(server, window);

	// Create opengl context.
	gl = glXCreateContext(server, vi, NULL, GL_TRUE);
	if (gl == 0)
		throw std::runtime_error("failed to create OpenGL context");
	
	// Switch to opengl context.
	if (!glXMakeCurrent(server, window, gl))
		throw std::runtime_error("failed to switch OpenGL context");

	// Carry out the above tasks.
	XFlush(server);

	// Listen for window manager's close event.
	wm_del = XInternAtom(server, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(server, window, &wm_del, 1);

	// Make sure input is going to window.
	XSetInputFocus(server, window, None, CurrentTime);

	// Prime OpenGL for rendering.
	glClearColor(1, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	remap();
}

void X11Display::remap()
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);

#ifdef DEBUG
	std::cout << "x11_display: Remapped to " << width << "x" << height << std::endl;
#endif
}

bool X11Display::draw(const Raster &raster)
{
	XEvent event;
	while (XPending(server) > 0) {
		XNextEvent(server, &event);
		switch (event.type) {
			// The window was resized.
			case ConfigureNotify:
				if (width == event.xconfigure.width
				  && height == event.xconfigure.height)
					break;
				width = event.xconfigure.width;
				height = event.xconfigure.height;
				remap();
				break;
			// Message from window manager.
			case ClientMessage:
				// The close button on the window was pushed.
				if( event.xclient.data.l[0] == (long)wm_del )
					return false;
				break;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, raster.width(), raster.height(),
	             0, GL_RGBA, GL_UNSIGNED_BYTE, raster.pixels());
	
	// TODO: Replace with vertex buffer.
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1);
	glVertex2i(0, 0);
	glTexCoord2i(0, 1);
	glVertex2i(width, 0);
	glTexCoord2i(0, 0);
	glVertex2i(width, height);
	glTexCoord2i(1, 0);
	glVertex2i(0, height);
	glEnd();

	glXSwapBuffers(server, window);
	XFlush(server);
	return true;
}

X11Display::~X11Display()
{
	if (server == nullptr)
		return;

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
