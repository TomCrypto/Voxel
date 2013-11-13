#pragma once

// defines some stuff useful for pseudorandom sampling

#include <cstddef>

namespace math
{
    float uniform(void);

    float halton(size_t index, size_t base);
};
