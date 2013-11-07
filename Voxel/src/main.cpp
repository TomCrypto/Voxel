#include <cstdlib>

#ifdef TESTING

#include "testing/testing.h"

int main(int argc, char* argv[])
{
    Testing::RunTests();
    return EXIT_SUCCESS;
}

#else

// main program goes here

#include "renderer.h"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    std::cout << "Hello world" << std::endl;

	return EXIT_SUCCESS;
}

#endif
