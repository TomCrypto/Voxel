#include "math/random.hpp"

#include <random>
#include <omp.h>
#include <map>

static std::map<int, std::mt19937> prng;
static std::uniform_real_distribution<float> uniform_distribution(0, 1);

float math::uniform(void)
{
    int t = omp_get_thread_num();

    if (prng.find(t) == prng.end())
        prng.insert(std::make_pair(t, std::mt19937(t)));
        
    return uniform_distribution(prng[t]);
}

float math::halton(size_t index, size_t base)
{
    float result = 0;
    float f = 1.0f / base;

    while (index > 0)
    {
        result += f * (index % base);
        index /= base;
        f /=  base;
    }

    return result;
}
