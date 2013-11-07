#include "testing.h"

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

//#include "float4.h"
//#include "float4x4.h"

/******************************************************************************
******************* Add references to your unit tests here. *******************
******************************************************************************/

std::map<std::string, UNIT_TEST> AcquireAllTests()
{
    std::map<std::string, UNIT_TEST> tests;

    //tests["float4_access"]                       = Testing::float4_access;
    //tests["float4_add"]                          = Testing::float4_add;
    //tests["float4_sub"]                          = Testing::float4_sub;

    return tests;
}