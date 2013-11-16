#pragma once

#include "math/vector3.hpp"

// this again depends on what type of scalar is used to represent positions,
// but for now they are floats to keep it simple

// this class could contain things like "move player forward", etc.. all they
// need to do is provide position/view direction for the projection models,
// then the rest is just convenience functions for the GUI
struct Observer
{
    public:
        math::float3 position;
        math::float3 direction;
        float fov;
    
    private:
    
};
