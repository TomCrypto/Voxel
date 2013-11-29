/** @file atb.hpp
  *
  * @brief AntTweakBar Wrapper
  *
  * A very simple wrapper for the AntTweakBar library, which provides functions
  * to create variables of different types, check if the user has changed them,
  * and read/write them.
  *
  * This wrapper is designed to be very simple to use and lightweight. This was
  * not meant for general purpose use, and should probably not be reused.
**/

#pragma once

#include <SFML/Window.hpp>
#include <AntTweakBar.h>

namespace atb
{
    /** The variable type enumerating subsamplers.
    **/
    TwType subsamplers(void);

    /** The variable type enumerating projections.
    **/
    TwType projections(void);

    /** The variable type enumerating integrators.
    **/
    TwType integrators(void);

    /** Initializes the AntTweakBar library and creates one bar.
      *
      * @param bar_title  Title of the bar.
    **/
    void initialize(const char *bar_title);

    /** Informs the AntTweakBar library of a window resize.
      *
      * @param width   The new window width.
      * @param height  The new window height.
    **/
    void window_resize(std::size_t width, std::size_t height);

    /** Adds a variable to the tweak bar.
      *
      * @param id      The variable ID (used to refer to it, must be unique).
      * @param name    The variable name, as displayed inside the tweak bar.
      * @param type    The variable type (see AntTweakBar documentation).
      * @param options Additional AntTweakBar options.
      *
      * @remarks Not all types are supported, only the most common.
    **/
    void add_var(const char *id, const char *name, TwType type,
                 const char *options = "");

    /** Checks if a variable has changed since the last call to \c read_var().
      *
      * @param id  The variable ID.
      *
      * @return \c true if it has changed, \c false otherwise.
    **/
    bool has_changed(const char *id);

    /** Reads the value of a variable, returning a pointer to it.
      *
      * @param id  The variable ID.
      *
      * @return A pointer to the variable value.
      *
      * @remarks This call resets the modification flag on this variable.
    **/
    const void *read_var(const char *id);

    /** Writes the value of a variable.
      *
      * @param id   The variable ID.
      * @param ptr  A pointer to the new value.
    **/
    void write_var(const char *id, const void *ptr);

    /** @see #read_var()
    **/
    template <typename T>
    const T get_var(const char *id)
    {
        return *(T*)read_var(id);
    }

    /** @see #write_var()
    **/
    template <typename T>
    void set_var(const char *id, const T &data)
    {
        write_var(id, &data);
    }

    /** Lets the AntTweakBar library process an SFML event.
      *
      * @param event  The SFML event.
      *
      * @return \c true if the event was handled, \c false otherwise.
    **/
    bool handle_event(sf::Event &event);

    /** Draws the tweak bar on the screen.
    **/
    void draw(void);

    /** Finalizes the AntTweakBar library.
    **/
    void finalize(void);
};
