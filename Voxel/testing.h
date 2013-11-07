#pragma once

/* This namespace is reserved for unit testing. */
namespace Testing
{
    /* Causes the test to fail if x does not hold. */
    #define TEST_ASSERT(x) if (!(x)) return false;

    /* Run all the unit tests. */
    void RunTests(void);
}