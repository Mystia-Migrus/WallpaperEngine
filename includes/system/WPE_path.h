#pragma once
#ifndef WPE_PATH_H
#define WPE_PATH_H
#include <system/WPE_base.h>

WPE_NAMESPACE_BEGIN
namespace Path {
    class Path {
    public:
        Path() = default;
        Path(const std::string& init_path);
        
        
    private:
        std::string path;
    
    };
}
WPE_NAMESPACE_END
#endif