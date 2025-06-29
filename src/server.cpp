#include <sdbus-c++/sdbus-c++.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <map>

namespace fs = std::filesystem;
using json = nlohmann::json;

class ApplicationConfiguration
{
public:
    ApplicationConfiguration(const std::string& appName, const std::string& configPath, sdbus::IConnection& connection)
        : appName_(appName), configPath_(configPath)
    {
        loadConfig();

        sdbus::ObjectPath objectPath{"/com/system/configurationManager/Application/" + appName_};
        dbusObject_ = sdbus::createObject(connection, objectPath);

        dbusObject_->addVTable(
                sdbus::registerMethod("ChangeConfiguration")
                .implementedAs([this](const std::string& key, const sdbus::Variant& value) {
                    this->changeConfig(key, value);
                }),
                sdbus::registerMethod("GetConfiguration")
                .implementedAs([this]() {
                    return this->getConfig();
                }),
                sdbus::registerSignal("configurationChanged")
                .withParameters<std::map<std::string, sdbus::Variant>>())
                .forInterface("com.system.configurationManager.Application.Configuration");

        
    }

    void loadConfig() {
        try {
            std::ifstream file(configPath_);
            if (!file) throw std::runtime_error("File not found");
            json configJson = json::parse(file);
            config_.clear();
            for (auto& [key, value] : configJson.items()) {
                config_[key] = sdbus::Variant{value.get<std::string>()};
            }
        } catch (const std::exception& e) {
            throw sdbus::Error(sdbus::Error::Name{"LoadFailed"}, "Failed to load configuration: " + std::string(e.what()));
        }
    }

    void saveConfig() {
        try {
            json configJson;
            for (auto& [key, variant] : config_) {
                configJson[key] = variant.get<std::string>();
            }
            std::ofstream file(configPath_);
            file << configJson.dump(4);
        } catch (const std::exception& e) {
            throw sdbus::Error(sdbus::Error::Name{"SaveFailed"}, "Failed to save configuration: " + std::string(e.what()));
        }
    }

    void changeConfig(const std::string& key, const sdbus::Variant& value) {
        config_[key] = value;
        saveConfig();

        dbusObject_->emitSignal("configurationChanged")
            .onInterface("com.system.configurationManager.Application.Configuration")
            .withArguments(config_);
    }

    std::map<std::string, sdbus::Variant> getConfig() {
        return config_;
    }

private:
    std::string appName_;
    std::string configPath_;
    std::map<std::string, sdbus::Variant> config_;
    std::unique_ptr<sdbus::IObject> dbusObject_;
};


int main() {
    const char* home_dir = std::getenv("HOME");
    if (!home_dir) {
        throw std::runtime_error("Error: HOME environment variable not found!");
        return 1;
    }

    sdbus::ServiceName serviceName{"com.system.configurationManager"};
    auto connection = sdbus::createBusConnection(serviceName);

    fs::path configDir = std::string(getenv("HOME")) + "/com.system.configurationManager";
    std::vector<std::unique_ptr<ApplicationConfiguration>> apps;

    if (!fs::exists(configDir)) {
            if (!fs::create_directories(configDir)) {
                throw std::runtime_error("Failed to create directory: " + configDir.string());
            }
        }

    for (const auto& entry: fs::directory_iterator(configDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            apps.emplace_back(std::make_unique<ApplicationConfiguration>(
                entry.path().stem(), entry.path(), *connection
            ));
        }
    }

    connection->enterEventLoop();
    return 0;
}