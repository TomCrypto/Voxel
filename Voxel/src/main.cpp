#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include "renderer.hpp"

#ifndef LEAN
#include "testing/testing.hpp"
#endif

int main(int argc, char* argv[])
{
    if ((argc == 2) && (!strcmp(argv[1], "--test")))
    {
        #ifndef LEAN
        Testing::RunTests();
        #endif
        return EXIT_SUCCESS;
    }

    // program goes here

    std::cout << "Hello world" << std::endl;

    return EXIT_SUCCESS;
}
