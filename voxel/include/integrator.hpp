#pragma once

/* Indicative interface for the Integrator */

class Integrator
{
    private:
        // some minimal state, but the integrator should be mostly stateless
        // given that it must be thread-safe

    public:
        Integrator(Geometry geometry);

        // returns the color as perceived along a given ray (camera ray,
        // but the integrator can recursively call this function if needed)
        Color integrate(vector3 origin, vector3 direction);
};
