#pragma once
#ifndef WPE_CONFIG_H
#define WPE_CONFIG_H
#include <system/WPE_base.h>

WPE_NAMESPACE_BEGIN
namespace Config {
    struct AppConfig {
        unsigned int frame_rate;
    };

    bool LoadConfig(const std::string& filename, AppConfig& config);

    bool SaveConfig(const std::string& filename, const AppConfig& config);
}
WPE_NAMESPACE_END
#endif