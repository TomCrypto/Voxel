// anttweakbar namespace (atb::) goes in here
// should manage a single bar, and make it EASY and SIMPLE to do it
// needed options:
// - create variable
// - know when a variable has been changed by the user

#pragma once

#include <SFML/Window.hpp>
#include <AntTweakBar.h>

namespace atb
{
    TwType subsamplers(void);
    TwType projections(void);
    TwType integrators(void);

    void initialize(const char *bar_title);

    void window_resize(size_t width, size_t height);

    void add_var(const char *id, const char *name, TwType type,
                 const char *options = "");

    bool has_changed(const char *id);

    const void *read_var(const char *id);

    void write_var(const char *id, const void *ptr);

    template <typename T>
    const T get_var(const char *id)
    {
        return *(T*)read_var(id);
    }

    template <typename T>
    void set_var(const char *id, const T &data)
    {
        write_var(id, &data);
    }

    bool handle_event(sf::Event &event);

    void draw(void);

    void finalize(void);
};
