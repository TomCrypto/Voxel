#include "gui/atb.hpp"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "modules/subsamplers.hpp"
#include "modules/projections.hpp"
#include "modules/integrators.hpp"

using std::unique_ptr;

static TwType subsamplers_t, projections_t, integrators_t;
TwType atb::subsamplers(void) { return subsamplers_t; }
TwType atb::projections(void) { return projections_t; }
TwType atb::integrators(void) { return integrators_t; }

static void TW_CALL set_cb(const void *value, void *id);
static void TW_CALL get_cb(      void *value, void *id);
static TwBar *bar;

struct Variable
{
    private:
        std::vector<char> var_id;
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
            subsamplers::modules v_subsampler;
            projections::modules v_projection;
            integrators::modules v_integrator;
        } data;

        std::size_t get_type_size(TwType type)
        {
            if (type == TW_TYPE_BOOLCPP)    return sizeof(data.v_bool);
            if (type == TW_TYPE_INT32)      return sizeof(data.v_int);
            if (type == TW_TYPE_UINT32)     return sizeof(data.v_uint);
            if (type == TW_TYPE_FLOAT)      return sizeof(data.v_float);
            if (type == TW_TYPE_COLOR3F)    return sizeof(data.v_color3f);
            if (type == TW_TYPE_COLOR4F)    return sizeof(data.v_color4f);
            if (type == TW_TYPE_DIR3F)      return sizeof(data.v_dir3f);
            if (type == atb::subsamplers()) return sizeof(data.v_subsampler);
            if (type == atb::projections()) return sizeof(data.v_projection);
            if (type == atb::integrators()) return sizeof(data.v_integrator);
            throw new std::logic_error("Unknown TweakBar variable type");
        }

    public:
        bool changed;

        Variable(const char *id, const char *name, TwType type,
                 const char *opt = "")
        {
            if (!strlen(id)) // Cannot accept empty variable names
                throw std::logic_error("Variable must have an ID");

            this->changed = false;
            this->size = get_type_size(type);
            this->var_id = std::vector<char>(id, id + strlen(id) + 1);
            auto def = (std::string(opt) + " label='" + name + "'").c_str();
            if (!TwAddVarCB(bar, id, type, set_cb, get_cb, &var_id[0], def))
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

        void set_var(const void *ptr, bool change = true)
        {
            if (memcmp(&data, ptr, size))
            {
                memcpy(&data, ptr, size);
                this->changed = change;
            }
        }

        ~Variable()
        {
            TwRemoveVar(bar, &var_id[0]);
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
        {subsamplers::modules::NONE,        "None"},
        {subsamplers::modules::AAx2,        "2xAA"},
        {subsamplers::modules::AAx4,        "4xAA"},
        {subsamplers::modules::AAx8,        "8xAA"},
    },   subsamplers::modules::COUNT_);

    projections_t = TwDefineEnum("Projection", (const TwEnumVal[])
    {
        {projections::modules::PERSPECTIVE, "Perspective"},
    },   projections::modules::COUNT_);

    integrators_t = TwDefineEnum("Integrator", (const TwEnumVal[])
    {
        {integrators::modules::DEPTH,       "Depth"},
        {integrators::modules::AO,          "Ambient Occlusion"},
    },   integrators::modules::COUNT_);
}

void atb::initialize(const char *bar_title)
{
    if (!TwInit(TW_OPENGL_CORE, 0))
        throw std::runtime_error("Failed to initialize AntTweakBar");
    if (!(bar = TwNewBar(bar_title)))
        throw std::runtime_error("Failed to create new TweakBar");
    define_types();

    std::string prefix = "'" + std::string(bar_title) + "' ";
    TwDefine((prefix +  "contained=true").c_str());
    TwDefine((prefix +  "size='275 400'").c_str());
    TwDefine((prefix + "valueswidth=120").c_str());
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
    get_variable(id)->set_var(ptr, false);
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
