#include "devices.hpp"

#include "rlutil.h"
#include "log.hpp"

#include <algorithm>
#include <sstream>

using namespace rlutil;
using std::string;
using std::vector;
using std::cout;

/* These are some string utility functions (mainly for parsing and IO needs). */

static string ltrim(const string &s)
{
    string tmp = s;
    tmp.erase(tmp.begin(), std::find_if(tmp.begin(), tmp.end(),
              std::not1(std::ptr_fun<int, int>(std::isspace))));
    return tmp;
}

static string rtrim(const string &s)
{
    string tmp = s;
    tmp.erase(std::find_if(tmp.rbegin(), tmp.rend(),
              std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
              tmp.end());
    return tmp;
}

static string trim(const string &s)
{
    return ltrim(rtrim(s));
}

static vector<string> &split(const string &s, char delim,
                             vector<string> &elems)
{
    string item;
    std::stringstream ss(s);
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

static vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

static bool starts_with(const string &s, const string &p)
{
    return !strncmp(p.c_str(), s.c_str(), p.length());
}

static size_t parse_int(const string &s)
{
    try
    {
        return std::stoi(s);
    }
    catch (...)
    {
        return (size_t)-1;
    }
}

/* The following are some utility functions for getting platform/device info. */

static vector<cl::Device> get_devices(const cl::Platform &platform)
{
    vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    return devices;
}

static string get_name(const cl::Platform &platform)
{
    return trim(platform.getInfo<CL_PLATFORM_NAME>());
}

static string get_name(const cl::Device &device)
{
    return trim(device.getInfo<CL_DEVICE_NAME>());
}

static string get_version(const cl::Device &device)
{
    return trim(device.getInfo<CL_DEVICE_VERSION>());
}

static string get_extensions(const cl::Device &device)
{
    return trim(device.getInfo<CL_DEVICE_EXTENSIONS>());
}

static cl_device_type get_type(const cl::Device &device)
{
    return device.getInfo<CL_DEVICE_TYPE>();
}

/* This function checks for OpenCL 1.2 support by using the CL_DEVICE_VERSION *
 * string provided by the device. If this string differs from the format that *
 * is mandated by the OpenCL specification, this method will invoke undefined *
 * behaviour.                                                                 */
static bool check_version(const cl::Device &device)
{
    string version = get_version(device);
    version.erase(0, strlen("OpenCL "));
    auto pos = version.find(' ');
    if (pos != string::npos)
        version.erase(pos);

    auto v = split(version, '.');
    int major = std::stoi(v[0]);
    int minor = std::stoi(v[1]);

    return (major == 1 ? minor >= 2 : major > 1);
}

/* This function checks that the device supports OpenCL-OpenGL interop, which *
 * is required for the renderer to be able to display frames interactively.   */
static bool check_gl_interop(const cl::Device &device)
{
    vector<string> extensions = split(get_extensions(device), ' ');
    for (auto it = extensions.begin(); it != extensions.end(); ++it)
        if (*it == "cl_khr_gl_sharing") return true;

    return false;
}

/* This function checks this device supports the full (not embedded) profile. */
static bool check_full_profile(const cl::Device &device)
{
    return device.getInfo<CL_DEVICE_PROFILE>() == "FULL_PROFILE";
}

/* This function ensures the device supports images, we use 2D and 2D arrays. */
static bool check_image_support(const cl::Device &device)
{
    return device.getInfo<CL_DEVICE_IMAGE_SUPPORT>();
}

/* This function checks if a given device "can run" the renderer in the sense *
 * that the device claims to support all required features (note it may still *
 * fail due to driver bugs or other reasons out of our control).              */
static bool is_valid_device(const cl::Device &device)
{
    return check_version(device)
        && check_gl_interop(device)
        && check_full_profile(device)
        && check_image_support(device);
}

/* This function parses a device ID or name by the user and checks if a given *
 * device matches this ID (only one device should ever match it).             */
static bool check_device(const cl::Platform &platform,
                         const cl::Device &device,
                         size_t p_id, size_t d_id,
                         const string &name)
{
    if (name.length() < 2) return false;

    if (starts_with(name, "cl"))
    {
        vector<string> pd = split(split(name, '/')[1], ':');
        return (pd.size() == 2) && (parse_int(pd[0]) == p_id)
                                && (parse_int(pd[1]) == d_id);
    }
    else
    {
        return name == get_name(device) + " (" + get_name(platform) + ")";
    }
}

static void print_device_id(size_t p_id, size_t d_id)
{
    cout << "cl/" << p_id << ":" << d_id;
}

static void print_type(cl_device_type &type, cl_device_type target,
                       int tag_color, int default_color,
                       const string &tag_name)
{
    if ((type & target) != 0)
    {
        setColor(tag_color);
        cout << tag_name;
        setColor(default_color);
        if (type ^= target) cout << "/";
    }
}

static void print_device_type(cl_device_type type)
{
    setColor(WHITE);
    cout << "[";

    print_type(type, CL_DEVICE_TYPE_CPU,             YELLOW, WHITE, "CPU");
    print_type(type, CL_DEVICE_TYPE_GPU,         LIGHTGREEN, WHITE, "GPU");
    print_type(type, CL_DEVICE_TYPE_ACCELERATOR,   LIGHTRED, WHITE, "ACC");
    print_type(type, CL_DEVICE_TYPE_DEFAULT,      LIGHTCYAN, WHITE, "DEF");
    print_type(type, CL_DEVICE_TYPE_CUSTOM,       LIGHTBLUE, WHITE, "CUS");

    setColor(WHITE);
    cout << "] ";
}

static void print_device_name(const cl::Platform &platform,
                              const cl::Device &device)
{
    setColor(LIGHTMAGENTA);
    cout << get_name(device) << " (" << get_name(platform) << ")";
}

static void print_device(const cl::Platform &platform,
                         const cl::Device &device,
                         size_t p_id, size_t d_id)
{
    setColor(is_valid_device(device) ? GREEN : RED);
    print_device_id(p_id, d_id);
    setColor(WHITE);
    cout << " -- ";

    print_device_type(get_type(device));
    print_device_name(platform, device);
    setColor(WHITE);
    cout << "\n";
}

static void print_selected_device(const cl::Platform &platform,
                                  const cl::Device &device)
{
    print_info_header();
    setColor(WHITE);
    cout << "Using ";
    print_device_name(platform, device);
    setColor(WHITE);
    cout << ".\n";
}

bool print_devices(void)
{
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (auto p = platforms.begin(); p != platforms.end(); ++p)
    {
        vector<cl::Device> devices = get_devices(*p);

        for (auto d = devices.begin(); d != devices.end(); ++d)
        {
            size_t p_id = p - platforms.begin();
            size_t d_id = d - devices.begin();
            print_device(*p, *d, p_id, d_id);
        }
    }

    return true;
}

bool select_device(string name, cl::Device &device)
{
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (auto p = platforms.begin(); p != platforms.end(); ++p)
    {
        vector<cl::Device> devices = get_devices(*p);

        for (auto d = devices.begin(); d != devices.end(); ++d)
        {
            size_t p_id = p - platforms.begin(), d_id = d - devices.begin();
            if (check_device(*p, *d, p_id, d_id, trim(name)))
            {
                print_selected_device(*p, *d);
                if (!is_valid_device(*d))
                {
                    /* Always attempt to use the device as required. */
                    print_warning("Device does not meet requirements");
                }

                device = *d;
                return true;
            }
        }
    }

    print_error("Device not found");
    return false;
}
