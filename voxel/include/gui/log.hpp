/** @file log.hpp
  *
  * @brief Logging Utilities
  *
  * This unit provides some simple console logging output for the program.
**/

#pragma once

#include <CL/cl.hpp>
#include <stdexcept>
#include <string>

/** Prints out an OpenCL exception to the terminal.
  *
  * @param msg  A contextual message string.
  * @param e    The exception to display.
**/
void print_exception(const std::string &msg, const cl::Error &e);

/** Prints out a generic exception to the terminal.
  *
  * @param msg  A contextual message string.
  * @param e    The exception to display.
**/
void print_exception(const std::string &msg, const std::exception &e);

/** Prints an error message to the terminal.
  *
  * @param msg  The error to display.
**/
void print_error(const std::string &msg);

/** Prints a warning to the terminal.
  *
  * @param msg  The warning to display.
**/
void print_warning(const std::string &msg);

/** Logs some information to the terminal.
  *
  * @param msg  The message to display.
**/
void print_info(const std::string &msg);

/** Prints out only the error header (e.g. [ERROR]).
  *
  * @remarks Useful for stylized log output.
**/
void print_error_header(void);

/** Prints out only the warning header.
  *
  * @rsee #print_error_header()
**/
void print_warning_header(void);

/** Prints out only the information header.
  *
  * @rsee #print_error_header()
**/
void print_info_header(void);
