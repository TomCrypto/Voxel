#include "display.hpp"

#include "compile_settings.hpp"

typedef bool (*DISPLAY_DEVICE)(const std::vector<std::string> &description,
                               const Renderer<Integrator> &renderer);

static DISPLAY_DEVICE get_display(const std::vector<std::string> &description);

bool has_display(const std::vector<std::string> &description)
{
    return (get_display(description) != nullptr);
}

bool start_display(const std::vector<std::string> &description,
                   const Renderer<Integrator> &renderer)
{
    auto device = get_display(description);
    if (device == nullptr) return false;
    else return device(description,
                       renderer);
}

/******************************************************************************
********************* ADD YOUR DISPLAY DEVICE HEADERS HERE ********************
******************************************************************************/

#include "display/ppm_file.hpp"

/******************************************************************************
******************** ADD YOUR DISPLAY DEVICE FUNCTIONS HERE *******************
******************************************************************************/

DISPLAY_DEVICE get_display(const std::vector<std::string> &description)
{
    if (!description.empty())
    {
        /* The device name always goes first. */
        const std::string name = description[0];

        if (name == "ppm") return run_ppm;
    }

    return nullptr;
}
