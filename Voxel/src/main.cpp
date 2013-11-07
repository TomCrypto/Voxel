#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include "renderer.hpp"

#include "testing/testing.hpp"

int main(int argc, char* argv[])
{
    if ((argc == 2) && (!strcmp(argv[1], "--test")))
    {
        Testing::RunTests();
        return EXIT_SUCCESS;
    }

    // program goes here

    std::cout << "Hello world" << std::endl;

    return EXIT_SUCCESS;
}
