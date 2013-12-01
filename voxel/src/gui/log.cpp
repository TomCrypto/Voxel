#include "external/rlutil.h"
#include "gui/log.hpp"

using namespace rlutil;
using std::exception;
using std::string;
using std::cout;

void print_exception(const string &msg, const cl::Error &e)
{
    print_error_header();
    cout << msg << ": ";
    setColor(BLUE);
    cout << e.what();
    setColor(WHITE);
    cout << " [" << e.err() << "].\n";
}

void print_exception(const string &msg, const exception &e)
{
    print_error_header();
    cout << msg << ": ";
    setColor(BLUE);
    cout << e.what();
    setColor(WHITE);
    cout << ".\n";
}

void print_error(const string &msg)
{
    print_error_header();
    cout << msg << ".\n";
}

void print_warning(const string &msg)
{
    print_warning_header();
    cout << msg << ".\n";
}

void print_info(const string &msg)
{
    print_info_header();
    cout << msg << ".\n";
}

/* Log header functions. */

void print_error_header(void)
{
    setColor(LIGHTRED);
    cout << "[ FAIL ]";
    setColor(WHITE);
    cout << " ";
}

void print_warning_header(void)
{
    setColor(YELLOW);
    cout << "[ WARN ]";
    setColor(WHITE);
    cout << " ";
}

void print_info_header(void)
{
    setColor(LIGHTCYAN);
    cout << "[ INFO ]";
    setColor(WHITE);
    cout << " ";
}
