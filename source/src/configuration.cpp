#include "configuration.h"

#include <iostream>
#include <utility>

namespace Shower {

auto NoNameDefined::what() const noexcept -> const char*
{
    return "Config error: No name defined.";
}

auto ArgumentError::what() const noexcept -> const char*
{
    return "commandline argument error.";
}

auto FaultyDetectorDefinition::what() const noexcept -> const char*
{
    return "Config error: Faulty detector definition.";
}

auto FaultyAtmosphereLayerDefinition::what() const noexcept -> const char*
{
    return "Config error: Faulty atmosphere layer definition.";
}

auto FaultyPrimaryDefinition::what() const noexcept -> const char*
{
    return "Config error: Faulty primary definition.";
}

auto FaultySecondaryDefinition::what() const noexcept -> const char*
{
    return "Config error: Faulty secondary definition.";
}

auto FaultyMagneticFieldDefinition::what() const noexcept -> const char*
{
    return "Config error: Faulty magnetic field definition.";
}

Configuration::Configuration(std::string file_name)
    : m_file_name { std::move(file_name) }
{
    m_fallback.readFile("shower.cfg");

    m_config.readFile(m_file_name.c_str());

    if (!get_root().exists("name")) {
        throw NoNameDefined();
    }
}

auto Configuration::get_detectors(const bool& fallback) const -> std::variant<std::vector<Config::DetectorPlacement>, size_t>
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
        auto len = detectors_setting.getLength();
        for (int i = 0; i < len; i++) {
            const libconfig::Setting& detector = detectors_setting[i];
            Config::DetectorPlacement settings;
            if (!(detector.lookupValue("x", settings.x) && detector.lookupValue("y", settings.y) && detector.lookupValue("z", settings.z) && detector.lookupValue("name", settings.name))) {
                throw FaultyDetectorDefinition();
            }
            settings.x *= m;
            settings.y *= m;
            settings.z *= m;
            detectors.push_back(settings);
        }
        return std::move(detectors);
    }
    return static_cast<size_t>(number);
}

auto Configuration::get_atmosphere_layers(const bool& fallback) const -> std::vector<Config::AtmosphereLayer>
{
    if (!fallback) {
        if (!get_root().exists("layers")) {
            return get_atmosphere_layers(true);
        }
    }
    const libconfig::Setting& layers_setting = get_root(fallback)["layers"];
    std::vector<Config::AtmosphereLayer> layers;
    auto len = layers_setting.getLength();
    for (int i = 0; i < len; i++) {
        const libconfig::Setting& layer = layers_setting[i];
        Config::AtmosphereLayer settings {};
        if (!(layer.lookupValue("id", settings.id) && layer.lookupValue("lower", settings.lower) && layer.lookupValue("upper", settings.upper) && layer.lookupValue("density", settings.density) && layer.lookupValue("pressure", settings.pressure) && layer.lookupValue("temperature", settings.temperature))) {
            throw FaultyAtmosphereLayerDefinition();
        }
        settings.lower *= m;
        settings.upper *= m;
        settings.density *= kg / m3;
        settings.pressure *= pascal;
        settings.temperature *= kelvin;
        layers.push_back(settings);
    }
    return layers;
}

auto Configuration::get_particles(const bool& fallback) const -> std::vector<Config::SecondaryParticle>
{
    if (!fallback) {
        if (!get_root().exists("particles")) {
            return get_particles(true);
        }
    }
    const libconfig::Setting& particles_setting = get_root(fallback)["particles"];
    std::vector<Config::SecondaryParticle> particles;
    auto len = particles_setting.getLength();
    for (int i = 0; i < len; i++) {
        Config::SecondaryParticle settings {};
        settings.pdg = particles_setting[i];
        particles.push_back(settings);
    }
    return particles;
}

auto Configuration::get_primaries(const bool& fallback) const -> std::vector<Config::PrimaryParticle>
{
    if (!fallback) {
        if (!get_root().exists("primary")) {
            return get_primaries(true);
        }
    }

    const libconfig::Setting& primaries_setting = get_root(fallback)["primary"];
    std::vector<Config::PrimaryParticle> primaries;
    auto len = primaries_setting.getLength();
    for (int i = 0; i < len; i++) {
        const libconfig::Setting& primary_setting = primaries_setting[i];
        const libconfig::Setting& origin = primary_setting["origin"];
        const libconfig::Setting& momentum = primary_setting["momentum"];
        Config::PrimaryParticle primary {};
        if (!(origin.lookupValue("absolute", primary.origin.absolute) && origin.lookupValue("x", primary.origin.x) && origin.lookupValue("y", primary.origin.y) && origin.lookupValue("z", primary.origin.z) && momentum.lookupValue("x", primary.momentum.y) && momentum.lookupValue("y", primary.momentum.y) && momentum.lookupValue("z", primary.momentum.z) && momentum.lookupValue("magnitude", primary.momentum.m) && primary_setting.lookupValue("n_particles", primary.n_particles) && primary_setting.lookupValue("name", primary.name) && primary_setting.lookupValue("particle", primary.particle))) {
            throw FaultyPrimaryDefinition();
        }
        primary.origin.x *= m;
        primary.origin.y *= m;
        primary.origin.z *= m;
        primary.momentum.m *= MeV;
        primaries.push_back(primary);
    }
    return primaries;
}

auto Configuration::get_primary_event_limit(const bool& fallback) const -> double
{
    if (!fallback) {
        if (!get_root().exists("primary_event_limit")) {
            return get_primary_event_limit(true);
        }
    }
    return static_cast<double>(get_root(fallback)["primary_event_limit"]) * MeV;
}

auto Configuration::get_detector_properties(const bool& fallback) const -> Config::DetectorProperties
{
    if (!fallback) {
        if (!get_root().exists("detector_properties")) {
            return get_detector_properties(true);
        }
    }
    const libconfig::Setting& detector_setting = get_root(fallback)["detector_properties"];
    Config::DetectorProperties detector {};
    const libconfig::Setting& geometry = detector_setting["geometry"];
    const libconfig::Setting& physical = detector_setting["physical"];

    if (!(geometry.lookupValue("x", detector.geometry.x) && geometry.lookupValue("y", detector.geometry.y) && geometry.lookupValue("z", detector.geometry.z) && physical.lookupValue("z", detector.physical.z) && physical.lookupValue("a", detector.physical.a) && physical.lookupValue("rho", detector.physical.rho))) {
        throw FaultyDetectorDefinition();
    }
    detector.geometry.x *= mm;
    detector.geometry.y *= mm;
    detector.geometry.z *= mm;
    detector.physical.rho *= g / cm3;
    return detector;
}

auto Configuration::get_magnetic_field(const bool& fallback) const -> Config::MagneticField
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
    field.x *= tesla;
    field.y *= tesla;
    field.z *= tesla;
    return field;
}

auto Configuration::get_tracking_cut(const bool& fallback) const -> Config::TrackingCuts
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
    cuts.energy *= MeV;
    cuts.range *= m;
    return cuts;
}

auto Configuration::get_name() const -> std::string
{
    return m_config.lookup("name");
}

auto Configuration::get_events(const bool& fallback) const -> int
{
    if (!fallback) {
        if (!get_root().exists("events")) {
            return get_events(true);
        }
    }
    return get_root(fallback).lookup("events");
}

auto Configuration::get_data_directory(const bool& fallback) const -> std::string
{
    if (fallback || !m_config.exists("data_directory")) {
        return m_fallback.lookup("data_directory");
    }
    return m_config.lookup("data_directory");
}

auto Configuration::get_root(const bool& fallback) const -> const libconfig::Setting&
{
    if (fallback) {
        return m_fallback.getRoot();
    }
    return m_config.getRoot();
}

auto Configuration::get_world_size(const bool& fallback) const -> double
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
    return size * m;
}

auto Configuration::get_atmosphere_height(const bool& fallback) const -> double
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
    return upper * m;
}

void Configuration::add_detector(const Config::DetectorPlacement& detector)
{
    m_detectors.push_back(detector);
}

void Configuration::config_dump(const std::string& filename)
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
            group.add("x", libconfig::Setting::TypeFloat) = m_detector.x / meter;
            group.add("y", libconfig::Setting::TypeFloat) = m_detector.y / meter;
            group.add("z", libconfig::Setting::TypeFloat) = m_detector.z / meter;
            group.add("name", libconfig::Setting::TypeString) = m_detector.name;
        }
    }
    {
        root.add("primary_event_limit", libconfig::Setting::TypeFloat) = get_primary_event_limit();
    }
    {
        libconfig::Setting& detector_props = root.add("detector_properties", libconfig::Setting::TypeGroup);
        libconfig::Setting& geometry = detector_props.add("geometry", libconfig::Setting::TypeGroup);
        libconfig::Setting& physical = detector_props.add("physcial", libconfig::Setting::TypeGroup);
        auto det_prop = get_detector_properties();
        geometry.add("x", libconfig::Setting::TypeFloat) = det_prop.geometry.x / mm;
        geometry.add("y", libconfig::Setting::TypeFloat) = det_prop.geometry.y / mm;
        geometry.add("z", libconfig::Setting::TypeFloat) = det_prop.geometry.z / mm;
        physical.add("z", libconfig::Setting::TypeFloat) = det_prop.physical.z;
        physical.add("a", libconfig::Setting::TypeFloat) = det_prop.physical.a;
        physical.add("rho", libconfig::Setting::TypeFloat) = det_prop.physical.rho / (g / cm3);
    }
    {
        libconfig::Setting& primary_setting = root.add("primary", libconfig::Setting::TypeList);
        auto primaries = get_primaries();
        for (auto& prim : primaries) {
            libconfig::Setting& primary = primary_setting.add(libconfig::Setting::TypeGroup);
            libconfig::Setting& origin = primary.add("origin", libconfig::Setting::TypeGroup);
            libconfig::Setting& momentum = primary.add("momentum", libconfig::Setting::TypeGroup);
            origin.add("x", libconfig::Setting::TypeFloat) = prim.origin.x / meter;
            origin.add("y", libconfig::Setting::TypeFloat) = prim.origin.y / meter;
            origin.add("z", libconfig::Setting::TypeFloat) = prim.origin.z / meter;
            origin.add("absolute", libconfig::Setting::TypeBoolean) = prim.origin.absolute;

            momentum.add("x", libconfig::Setting::TypeFloat) = prim.momentum.x;
            momentum.add("y", libconfig::Setting::TypeFloat) = prim.momentum.y;
            momentum.add("z", libconfig::Setting::TypeFloat) = prim.momentum.z;
            momentum.add("magnitude", libconfig::Setting::TypeFloat) = prim.momentum.m / MeV;

            primary.add("n_particles", libconfig::Setting::TypeInt) = prim.n_particles;
            primary.add("particle", libconfig::Setting::TypeInt) = prim.particle;
            primary.add("name", libconfig::Setting::TypeString) = prim.name;
        }
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
        group.add("energy", libconfig::Setting::TypeFloat) = cuts.energy / MeV;
        group.add("range", libconfig::Setting::TypeFloat) = cuts.range / meter;
    }
    {
        libconfig::Setting& group = root.add("magnetic_field", libconfig::Setting::TypeGroup);
        auto field = get_magnetic_field();
        group.add("x", libconfig::Setting::TypeFloat) = field.x / tesla;
        group.add("y", libconfig::Setting::TypeFloat) = field.y / tesla;
        group.add("z", libconfig::Setting::TypeFloat) = field.z / tesla;
    }
    {
        root.add("world_size", libconfig::Setting::TypeFloat) = get_world_size() / meter;
    }
    {
        libconfig::Setting& list = root.add("layers", libconfig::Setting::TypeList);
        auto layers = get_atmosphere_layers();
        for (auto& layer : layers) {
            libconfig::Setting& group = list.add(libconfig::Setting::TypeGroup);
            group.add("id", libconfig::Setting::TypeInt) = layer.id;
            group.add("lower", libconfig::Setting::TypeFloat) = layer.lower / meter;
            group.add("upper", libconfig::Setting::TypeFloat) = layer.upper / meter;
            group.add("density", libconfig::Setting::TypeFloat) = layer.density / (kg / m3);
            group.add("pressure", libconfig::Setting::TypeFloat) = layer.pressure / pascal;
            group.add("temperature", libconfig::Setting::TypeFloat) = layer.temperature / kelvin;
        }
    }

    save.writeFile(filename.c_str());
}
}
