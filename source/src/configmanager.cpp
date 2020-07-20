#include "configmanager.h"

#include <iostream>
#include <utility>

#include "parametermanager.h"

namespace Shower {
ConfigManager* ConfigManager::c_singleton = nullptr;

auto ConfigManager::singleton() -> ConfigManager*
{
    return c_singleton;
}

ConfigManager::ConfigManager(std::string file_name)
    : m_file_name { std::move(file_name) }
{
    c_singleton = this;

    m_fallback.readFile("shower.cfg");

    m_config.readFile(m_file_name.c_str());

    if (!get_root().exists("name")) {
        throw NoNameDefined();
    }
}

ConfigManager::~ConfigManager()
    = default;

auto ConfigManager::get_detectors(const bool& fallback) const -> std::variant<std::vector<Config::DetectorPlacement>, size_t>
{
    if (!fallback) {
        if (!get_root().exists("detectors")) {
            return get_detectors(true);
        }
    }
    unsigned int number;
    if (!get_root(fallback).lookupValue("detectors", number)) {
        const libconfig::Setting& detectors_setting = get_root(fallback)["detectors"];
        std::vector<Config::DetectorPlacement> detectors;
        size_t len = detectors_setting.getLength();
        for (size_t i = 0; i < len; i++) {
            const libconfig::Setting& detector = detectors_setting[i];
            Config::DetectorPlacement settings;
            if (!(detector.lookupValue("x", settings.x) && detector.lookupValue("y", settings.y) && detector.lookupValue("z", settings.z) && detector.lookupValue("name", settings.name))) {
                throw FaultyDetectorDefinition();
            }
            detectors.push_back(settings);
        }
        return detectors;
    }
    return static_cast<size_t>(number);
}

auto ConfigManager::get_atmosphere_layers(const bool& fallback) const -> std::vector<Config::AtmosphereLayer>
{
    if (!fallback) {
        if (!get_root().exists("layers")) {
            return get_atmosphere_layers(true);
        }
    }
    const libconfig::Setting& layers_setting = get_root(fallback)["layers"];
    std::vector<Config::AtmosphereLayer> layers;
    size_t len = layers_setting.getLength();
    for (size_t i = 0; i < len; i++) {
        const libconfig::Setting& layer = layers_setting[i];
        Config::AtmosphereLayer settings {};
        if (!(layer.lookupValue("lower", settings.lower) && layer.lookupValue("upper", settings.upper) && layer.lookupValue("density", settings.density) && layer.lookupValue("pressure", settings.pressure) && layer.lookupValue("temperature", settings.temperature))) {
            throw FaultyAtmosphereLayerDefinition();
        }
        layers.push_back(settings);
    }
    return layers;
}

auto ConfigManager::get_particles(const bool& fallback) const -> std::vector<Config::SecondaryParticle>
{
    if (!fallback) {
        if (!get_root().exists("particles")) {
            return get_particles(true);
        }
    }
    const libconfig::Setting& particles_setting = get_root(fallback)["particles"];
    std::vector<Config::SecondaryParticle> particles;
    size_t len = particles_setting.getLength();
    for (size_t i = 0; i < len; i++) {
        Config::SecondaryParticle settings {};
        settings.pdg = particles_setting[i];
        particles.push_back(settings);
    }
    return particles;
}

auto ConfigManager::get_primary_particle(const bool& fallback) const -> Config::PrimaryParticle
{
    if (!fallback) {
        if (!get_root().exists("primary")) {
            return get_primary_particle(true);
        }
    }
    const libconfig::Setting& primary_setting = get_root(fallback)["primary"];
    Config::PrimaryParticle primary {};
    const libconfig::Setting& origin = primary_setting["origin"];
    const libconfig::Setting& momentum = primary_setting["momentum"];

    if (!(origin.lookupValue("x", primary.origin.x) && origin.lookupValue("y", primary.origin.y) && origin.lookupValue("z", primary.origin.z) && momentum.lookupValue("x", primary.momentum.y) && momentum.lookupValue("y", primary.momentum.y) && momentum.lookupValue("z", primary.momentum.z) && momentum.lookupValue("magnitude", primary.momentum.m) && primary_setting.lookupValue("n_particles", primary.n_particles) && primary_setting.lookupValue("particle", primary.particle))) {
        throw FaultyPrimaryDefinition();
    }
    return primary;
}
auto ConfigManager::get_primary_event_limit(const bool& fallback) const -> double
{
    if (!fallback) {
        if (!get_root().exists("primary_event_limit")) {
            return get_primary_event_limit(true);
        }
    }
    return static_cast<double>(get_root(fallback)["initial_ratio"]);
}

auto ConfigManager::get_detector_properties(const bool& fallback) const -> Config::DetectorProperties
{
    if (!fallback) {
        if (!get_root().exists("detector_properties")) {
            return get_detector_properties(true);
        }
    }
    const libconfig::Setting& detector_setting = get_root(fallback)["detector_properties"];
    Config::DetectorProperties detector {};
    const libconfig::Setting& geometry = detector_setting["geometry"];

    if (!(geometry.lookupValue("x", detector.geometry.x) && geometry.lookupValue("y", detector.geometry.y) && geometry.lookupValue("z", detector.geometry.z))) {
        throw FaultyDetectorDefinition();
    }
    return detector;
}

auto ConfigManager::get_magnetic_field(const bool& fallback) const -> Config::MagneticField
{
    if (!fallback) {
        if (!get_root().exists("magnetic_field")) {
            return get_magnetic_field(true);
        }
    }
    const libconfig::Setting& field_setting = get_root(fallback)["magnetic_field"];
    Config::MagneticField field {};

    if (!(field_setting.lookupValue("x", field.x) && field_setting.lookupValue("y", field.y) && field_setting.lookupValue("z", field.z))) {
        throw FaultyMagneticFieldDefinition();
    }
    return field;
}

auto ConfigManager::get_tracking_cut(const bool& fallback) const -> Config::TrackingCuts
{
    if (!fallback) {
        if (!get_root().exists("cuts")) {
            return get_tracking_cut(true);
        }
    }
    const libconfig::Setting& cuts_setting = get_root(fallback)["cuts"];
    Config::TrackingCuts cuts {};

    if (!(cuts_setting.lookupValue("energy", cuts.energy) && cuts_setting.lookupValue("range", cuts.range))) {
        throw FaultyMagneticFieldDefinition();
    }
    return cuts;
}

auto ConfigManager::get_name() const -> std::string
{
    return m_config.lookup("name");
}

auto ConfigManager::get_events(const bool& fallback) const -> int
{
    if (!fallback) {
        if (!get_root().exists("events")) {
            return get_events(true);
        }
    }
    return get_root(fallback).lookup("events");
}

auto ConfigManager::get_data_directory(const bool& fallback) const -> std::string
{
    if (fallback || !m_config.exists("data_directory")) {
        return m_fallback.lookup("data_directory");
    }
    return m_config.lookup("data_directory");
}

auto ConfigManager::get_root(const bool& fallback) const -> const libconfig::Setting&
{
    if (fallback) {
        return m_fallback.getRoot();
    }
    return m_config.getRoot();
}

auto ConfigManager::get_world_size(const bool& fallback) const -> double
{
    double size = 0;
    if (!fallback) {
        if (!get_root().exists("world_size")) {
            return get_world_size(true);
        }
    }
    if (!get_root(fallback).lookupValue("world_size", size)) {
        throw FaultyAtmosphereLayerDefinition();
    }
    return size;
}

auto ConfigManager::get_atmosphere_height(const bool& fallback) const -> double
{
    if (!fallback) {
        if (!get_root().exists("layers")) {
            return get_atmosphere_height(true);
        }
    }
    const libconfig::Setting& layers_setting = get_root(fallback)["layers"];
    double upper;
    int len = layers_setting.getLength();
    if (!(layers_setting[len - 1].lookupValue("upper", upper))) {
        throw FaultyAtmosphereLayerDefinition();
    }
    return upper;
}

void ConfigManager::add_detector(const Config::DetectorPlacement& detector)
{
    m_detectors.push_back(detector);
}

void ConfigManager::config_dump(const std::string& filename)
{
    libconfig::Config save;
    libconfig::Setting& root = save.getRoot();

    {
        root.add("name", libconfig::Setting::TypeString) = get_name();
    }
    {
        root.add("data_directory", libconfig::Setting::TypeString) = get_data_directory();
    }
    {
        root.add("events", libconfig::Setting::TypeInt) = get_events();
    }
    {
        libconfig::Setting& dets = root.add("detectors", libconfig::Setting::TypeList);
        for (auto& m_detector : m_detectors) {
            libconfig::Setting& group = dets.add(libconfig::Setting::TypeGroup);
            group.add("x", libconfig::Setting::TypeFloat) = m_detector.x;
            group.add("y", libconfig::Setting::TypeFloat) = m_detector.y;
            group.add("z", libconfig::Setting::TypeFloat) = m_detector.z;
            group.add("name", libconfig::Setting::TypeString) = m_detector.name;
        }
    }
    {
        root.add("primary_event_limit", libconfig::Setting::TypeFloat) = get_primary_event_limit();
    }
    {
        libconfig::Setting& detector_props = root.add("detector_properties", libconfig::Setting::TypeGroup);
        libconfig::Setting& geometry = detector_props.add("geometry", libconfig::Setting::TypeGroup);
        auto det_prop = get_detector_properties();
        geometry.add("x", libconfig::Setting::TypeFloat) = det_prop.geometry.x;
        geometry.add("y", libconfig::Setting::TypeFloat) = det_prop.geometry.y;
        geometry.add("z", libconfig::Setting::TypeFloat) = det_prop.geometry.z;
    }
    {
        libconfig::Setting& primary = root.add("primary", libconfig::Setting::TypeGroup);
        libconfig::Setting& origin = primary.add("origin", libconfig::Setting::TypeGroup);
        libconfig::Setting& momentum = primary.add("momentum", libconfig::Setting::TypeGroup);
        auto prim = get_primary_particle();
        origin.add("x", libconfig::Setting::TypeFloat) = prim.origin.x;
        origin.add("y", libconfig::Setting::TypeFloat) = prim.origin.y;
        origin.add("z", libconfig::Setting::TypeFloat) = prim.origin.z;

        momentum.add("x", libconfig::Setting::TypeFloat) = prim.momentum.x;
        momentum.add("y", libconfig::Setting::TypeFloat) = prim.momentum.y;
        momentum.add("z", libconfig::Setting::TypeFloat) = prim.momentum.z;
        momentum.add("magnitude", libconfig::Setting::TypeFloat) = prim.momentum.m;

        primary.add("n_particles", libconfig::Setting::TypeInt) = prim.n_particles;
        primary.add("particle", libconfig::Setting::TypeInt) = prim.particle;
    }
    {
        auto list = get_particles();
        libconfig::Setting& part = root.add("particles", libconfig::Setting::TypeList);
        for (auto& i : list) {
            part.add(libconfig::Setting::TypeInt) = i.pdg;
        }
    }
    {
        libconfig::Setting& group = root.add("cuts", libconfig::Setting::TypeGroup);
        auto cuts = get_tracking_cut();
        group.add("energy", libconfig::Setting::TypeFloat) = cuts.energy;
        group.add("range", libconfig::Setting::TypeFloat) = cuts.range;
    }
    {
        libconfig::Setting& group = root.add("magnetic_field", libconfig::Setting::TypeGroup);
        auto field = get_magnetic_field();
        group.add("x", libconfig::Setting::TypeFloat) = field.x;
        group.add("y", libconfig::Setting::TypeFloat) = field.y;
        group.add("z", libconfig::Setting::TypeFloat) = field.z;
    }
    {
        root.add("world_size", libconfig::Setting::TypeFloat) = get_world_size();
    }
    {
        libconfig::Setting& list = root.add("layers", libconfig::Setting::TypeList);
        auto layers = get_atmosphere_layers();
        for (size_t i = 0; i < layers.size(); i++) {
            libconfig::Setting& group = list.add(libconfig::Setting::TypeGroup);
            group.add("id", libconfig::Setting::TypeInt) = static_cast<int>(i);
            group.add("lower", libconfig::Setting::TypeFloat) = layers[i].lower;
            group.add("upper", libconfig::Setting::TypeFloat) = layers[i].upper;
            group.add("density", libconfig::Setting::TypeFloat) = layers[i].density;
            group.add("pressure", libconfig::Setting::TypeFloat) = layers[i].pressure;
            group.add("temperature", libconfig::Setting::TypeFloat) = layers[i].temperature;
        }
    }

    save.writeFile(filename.c_str());
}
}
