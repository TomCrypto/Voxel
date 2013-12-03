/** @file display.hpp
  *
  * @brief Graphical Display Interface
  *
  * This file provides an encapsulation of the graphical user interface.
**/

#pragma once

#include <SFML/Window.hpp>
#include <memory>
#include <string>

#include "world/world.hpp"

/** @namespace display
  *
  * @brief Namespace for the display
  *
  * Contains functions relating to the graphical (SFML) display.
**/
namespace display
{
    /** Initializes the user interface (creates the window, and so on).
      *
      * @param title  The window title (useful for versioning).
      *
      * @return The SFML window of the user interface.
      *
      * @remarks This is done once, just after device selection.
    **/
    std::unique_ptr<sf::Window> initialize(const std::string &title);

    /** Executes the main loop of the user interface, and returns when the user
      * exits the program (or due to some other condition).
      *
      * @param window  The SFML window, as returned by \c initialize().
      * @param world   The world to use for rendering.
    **/
    void run(std::unique_ptr<sf::Window> &window, World &world);

    /** Finalizes the display, closing the window.
      *
      * @param window  The SFML window.
    **/
    void finalize(std::unique_ptr<sf::Window> &window);
};
