#pragma once

#include <Utils/Config/ConfigManager.h>
#include <Utils/Config/ConfigProviders/CLIConfigProvider.h>
#include <Utils/Config/ConfigProviders/JsonConfigProvider.h>
#include <Utils/Providers/FileSourceProvider.h>

#include <memory>
#include <string>

namespace pacman::core {

// Extract --configPath from command-line arguments.
// Returns the path if found, otherwise returns the defaultPath.
inline std::string extractConfigPath(int argc, char* argv[], const std::string& defaultPath) {
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--configPath") {
            return argv[i + 1];
        }
    }
    return defaultPath;
}

// Initialize and run ConfigManager with CLI and JSON providers.
// Extracts configPath from argv, creates providers, and runs config resolution.
// Returns a managed ConfigManager ready for use.
template <typename ConfigT>
std::unique_ptr<Utils::Config::ConfigManager<ConfigT, Utils::Config::ConfigProviders::CLIConfigProvider<ConfigT>,
                                             Utils::Config::ConfigProviders::JsonConfigProvider<ConfigT>>>
initializeConfigManager(int argc, char* argv[], const std::string& defaultConfigPath) {
    using CLIProvider = Utils::Config::ConfigProviders::CLIConfigProvider<ConfigT>;
    using JsonProvider = Utils::Config::ConfigProviders::JsonConfigProvider<ConfigT>;
    using Manager = Utils::Config::ConfigManager<ConfigT, CLIProvider, JsonProvider>;

    std::string configPath = extractConfigPath(argc, argv, defaultConfigPath);

    auto fileSource = std::make_unique<Utils::Providers::FileSourceProvider>(configPath);

    auto manager = std::make_unique<Manager>(std::make_unique<CLIProvider>(argc, argv),
                                             std::make_unique<JsonProvider>(std::move(fileSource)));
    manager->run();

    return manager;
}

}  // namespace pacman::core
