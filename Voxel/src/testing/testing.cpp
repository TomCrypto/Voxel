#include "testing/testing.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

typedef bool (*UNIT_TEST)();

static std::map<std::string, UNIT_TEST> AcquireAllTests();

void Testing::RunTests()
{
    std::map<std::string, UNIT_TEST> tests = AcquireAllTests();
    std::cout << "Detected " << tests.size() << " unit tests.";
    std::cout << std::endl << std::endl;

    size_t failed = 0;
    size_t passed = 0;

    for (auto it = tests.begin(); it != tests.end(); ++it)
    {
        std::cout << "\t";

        if (it->second())
        {
            std::cout << "PASS";
            ++passed;
        }
        else
        {
            std::cout << "FAIL";
            ++failed;
        }

        std::cout << " :: " << it->first << std::endl;
    }

    std::cout << std::endl << failed << " test(s) failed, ";
    std::cout <<              passed << " test(s) passed, ";
    std::cout << "all tests successfully run." << std::endl;
}

/******************************************************************************
******************* Add #includes for your unit tests here. *******************
******************************************************************************/

#include "testing/modules/vector3.hpp"

/******************************************************************************
******************* Add references to your unit tests here. *******************
******************************************************************************/

std::map<std::string, UNIT_TEST> AcquireAllTests()
{
    std::map<std::string, UNIT_TEST> tests;

    tests["vector3_add"]                          = Testing::vector3_add;

    return tests;
}
