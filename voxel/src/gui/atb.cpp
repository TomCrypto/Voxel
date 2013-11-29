#include "gui/atb.hpp"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <memory>
#include <map>

#include "subsamplers/generic.hpp"
#include "projections/generic.hpp"
#include "integrators/generic.hpp"

using std::unique_ptr;

static TwType subsamplers_t;
static TwType projections_t;
static TwType integrators_t;

TwType atb::subsamplers(void) { return subsamplers_t; }
TwType atb::projections(void) { return projections_t; }
TwType atb::integrators(void) { return integrators_t; }

static void TW_CALL set_cb(const void *value, void *id);
static void TW_CALL get_cb(void *value, void *id);
static TwBar *bar;

struct Variable
{
    private:
        std::string name;
        std::size_t size;

        union
        {
            bool v_bool;
            int32_t v_int;
            uint32_t v_uint;
            float v_float;
            float v_color3f[3];
            float v_color4f[4];
            float v_dir3f[3];
            integrators::generic v_integrator;
            subsamplers::generic v_subsampler;
            projections::generic v_projection;
        } data;

        std::size_t get_type_size(TwType type)
        {
            if (type == TW_TYPE_BOOLCPP)
                return sizeof(data.v_bool);
            if (type == TW_TYPE_INT32)
                return sizeof(data.v_int);
            if (type == TW_TYPE_UINT32)
                return sizeof(data.v_uint);
            if (type == TW_TYPE_FLOAT)
                return sizeof(data.v_float);
            if (type == TW_TYPE_COLOR3F)
                return sizeof(data.v_color3f);
            if (type == TW_TYPE_COLOR4F)
                return sizeof(data.v_color4f);
            if (type == TW_TYPE_DIR3F)
                return sizeof(data.v_dir3f);
            if (type == atb::integrators())
                return sizeof(data.v_integrator);
            if (type == atb::subsamplers())
                return sizeof(data.v_subsampler);
            if (type == atb::projections())
                return sizeof(data.v_projection);

            throw new std::logic_error("Unknown TweakBar variable type");
        }

    public:
        bool changed;

        Variable(const char *id, const char *name, TwType type,
                 const char *options)
        {
            if (!strlen(id)) // Cannot accept empty variable names
                throw std::logic_error("Variable must have an ID");

            this->changed = false;
            this->size = get_type_size(type);
            this->name = std::string(id); /* Keep strong reference to ID. */
            std::string def = std::string(options) + " label='" + name + "'";
            if (!TwAddVarCB(bar, id, type, set_cb, get_cb,
                            const_cast<char *>(this->name.c_str()),
                            def.c_str()))
                throw std::runtime_error("Failed to create variable");
        }

        const void *get_var()
        {
            return &data;
        }

        void get_var(void *ptr)
        {
            memcpy(ptr, &data, size);
            this->changed = false;
        }

        void set_var(const void *ptr)
        {
            if (memcmp(&data, ptr, size))
            {
                memcpy(&data, ptr, size);
                this->changed = true;
            }
        }

        ~Variable()
        {
            TwRemoveVar(bar, name.c_str());
        }
};

static std::map<std::string, unique_ptr<Variable>> variables;

static unique_ptr<Variable> &get_variable(const char *id)
{
    auto var = variables.find(id);
    if (var == variables.end())
        throw std::logic_error("Variable '" + std::string(id) + "' not found");

    return var->second;
}

void TW_CALL set_cb(const void *value, void *id)
{
    get_variable((char*)id)->set_var(value);
}

void TW_CALL get_cb(void *value, void *id)
{
    get_variable((char*)id)->get_var(value);
}

static void define_types(void)
{
    subsamplers_t = TwDefineEnum("Subsampler", (const TwEnumVal[])
    {
        {subsamplers::generic::NONE, "None"},
        {subsamplers::generic::AAx2, "2xAA"},
        {subsamplers::generic::AAx4, "4xAA"},
        {subsamplers::generic::AAx8, "8xAA"},
    }, 4);

    integrators_t = TwDefineEnum("Integrator", (const TwEnumVal[])
    {
        {integrators::generic::DEPTH, "Depth"},
        {integrators::generic::AO, "Ambient Occlusion"},
    }, 2);

    projections_t = TwDefineEnum("Projection", (const TwEnumVal[])
    {
        {projections::generic::PERSPECTIVE, "Perspective"},
    }, 1);
}

void atb::initialize(const char *bar_title)
{
    if (!TwInit(TW_OPENGL_CORE, 0))
        throw std::runtime_error("Failed to initialize AntTweakBar");
    if (!(bar = TwNewBar(bar_title)))
        throw std::runtime_error("Failed to create new TweakBar");
    define_types();
}

void atb::window_resize(std::size_t width, std::size_t height)
{
    if (!TwWindowSize(width, height))
        throw std::runtime_error("Failed to update TweakBar dimensions");
}

void atb::add_var(const char *id, const char *name, TwType type,
                  const char *options)
{
    auto var = unique_ptr<Variable>(new Variable(id, name, type, options));
    variables.insert(std::make_pair(id, std::move(var)));
}

bool atb::has_changed(const char *id)
{
    return get_variable(id)->changed;
}

const void *atb::read_var(const char *id)
{
    return get_variable(id)->get_var();
}

void atb::write_var(const char *id, const void *ptr)
{
    get_variable(id)->set_var(ptr);
}

bool atb::handle_event(sf::Event &event)
{
    return TwEventSFML(&event, 2, 1);
}

void atb::draw(void)
{
    if (!TwDraw()) throw std::runtime_error("Failed to draw TweakBar");
}

void atb::finalize(void)
{
    variables.clear();
    TwDeleteBar(bar);
    TwTerminate();
}
