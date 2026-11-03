#include <system/WPE_config.h>
#include <nlohmann/json.hpp>
#include <fstream>

using namespace WPE_NAMESPACE::Config;

WPE_NAMESPACE_BEGIN
namespace Config{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppConfig, frame_rate);
}
WPE_NAMESPACE_END

bool WPE_NAMESPACE::Config::LoadConfig(const std::string& filename, AppConfig& config) {
    std::ifstream file(filename);
    if(file.is_open()) {
        nlohmann::json json;
        file >> json;
        
        config = json.get<AppConfig>();

        return true;
    }

    return false;
}

bool WPE_NAMESPACE::Config::SaveConfig(const std::string& filename, const AppConfig& config) {
    std::ofstream file(filename);

    if(file.is_open()) {
        nlohmann::json json(config);
        file << json.dump();
        file.close();
        return true;
    }
    
    return false;
}