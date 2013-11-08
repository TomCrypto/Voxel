#pragma once

#include "renderer.hpp"

/** @class DisplayDevice
  * 
  * The DisplayDevice class is an interface for using arbitrary display devices
  * which lets the renderer's output be presented to any type of media, such as
  * an interactive graphical user interface or a multimedia file format.
**/ 
class DisplayDevice
{
    public:
        /** Presents a renderer on some display device.
          *
          * @param renderer The renderer to present.
          *
          * @return \c false if an error occurred, \c true otherwise.
          *
          * @remarks Devices should discard any and all state associated with a
          *          renderer once this function returns, and should be able to
          *          handle this function being called multiple times.
        **/
        virtual bool present(const Renderer &renderer) = 0;
        
        virtual ~DisplayDevice()
        {
            /* ... */
        }
};
