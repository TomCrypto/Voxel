#pragma once

/* Indicative interface for the Geometry */

// Note the Geometry is meant to be read-only, changes are performed
// through World and lazily streamed to the Geometry as needed
class Geometry
{
    private:
        // geometry data etc..

    public:
        // Where Contact contains ray-voxel information for the integrator
        bool traverse(vector3 origin, vector3 direction,
                      scalar *distance, Contact *contact);
};
