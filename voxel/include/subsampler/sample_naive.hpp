#pragma once

/* Default naive sampling scheme, which returns 1 sample point
 * per pixel. In other words, it doesn't do anything special. */
 
/* These functions work as follows: they prodxce a deterministic
 * sample point offset for every 0 <= s < n, returning false for
 * s >= n and true for s < n. When they return false, it means
 * there are no remaining sample point offsets available, and
 * the renderer should stop asking it for more. Thus n is the
 * amount of sample points this subpixel sampler defines. */

bool naive_offset(size_t s, float &dx, float &dy)
{
    if (s > 1) return false;

    dx = 0;
    dy = 0;
    return true;
}
