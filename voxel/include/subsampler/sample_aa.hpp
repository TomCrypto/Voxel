#pragma once

/* Performs simple x4 AA in a diamond pattern (for testing). */

// A better implementation could template this to handle arbitrarily
// many sample points, according to some low-discrepancy distribution.

bool aa_offset(size_t s, float &dx, float &dy)
{
    if (s >= 4) return false;

    switch (s)
    {
        case 0:
            dx = -0.5f;
            dy = 0;
            break;
            
        case 1:
            dx = +0.5f;
            dy = 0;
            break;
            
        case 2:
            dx = 0;
            dy = -0.5f;
            break;
            
        case 3:
            dx = 0;
            dy = +0.5f;
            break;
    }
    
    return true;
}
