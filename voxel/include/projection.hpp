#pragma once

/* Indicative interface for the Projection */

class Projection
{
    private:
        // some state and precomputed data can go here

    public:
        // RenderInfo contains stuff about the screen resolution, etc...
        // (this may not be needed, stand by)
        Projection(RenderInfo info, /* Observer observer */);

        // (u, v) -> normalized display coordinates
        // produces a camera ray appropriate to (u, v) coords
        void project(scalar u, scalar v, vector3 *origin, vector3 *direction);
};
