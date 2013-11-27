// manages rendering

#pragma once

class Renderer
{
    public:
        

        cl::Program subsampler;
        cl::Program projection;
        cl::Program integrator;
};
