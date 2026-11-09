#pragma once
#ifndef WPE_PATH_H
#define WPE_PATH_H
#include <system/WPE_base.h>

WPE_NAMESPACE_BEGIN
namespace Path {

    class Path {
    public:
        Path() = default;
        Path(const std::string& path);
        
        Path& Append(const std::string& path);
        
        void Clear();
        
        const std::string Native() const;
        const std::string& Driver() const;
        const std::string& Directory() const;
        const std::string& Filename() const;
        
        unsigned int GetDeepLength() const;
    public:
        Path& operator<<(const std::string& path);
        Path operator+(const std::string& path);
    
    private:
        std::string driver;
        std::string directory;
        std::string filename;

        unsigned int deep_length;
    };
}
WPE_NAMESPACE_END
#endif