#pragma once
#ifndef WPE_PATH_H
#define WPE_PATH_H
#include <system/WPE_base.h>

WPE_NAMESPACE_BEGIN
namespace Path {

    class Path {
    public:
        enum State {
            PATH_STATE_OK,
            PATH_STATE_SYNTAX_ERROR,
            PATH_STATE_PATH_EXIST
        };

        Path() = default;
        Path(const std::string& path);
        
        Path& Append(const std::string& path);
        Path Create(const std::string& path);

        void Clear();
        
        const std::string Native() const;
        const std::string& Directory() const;
        const std::string& Filename() const;
        const std::string& Extension() const;
        const std::string& Root() const;
        
    public:
        Path& operator<<(const std::string& path);
        Path operator+(const std::string& path);
    
    private:
        static void AnalysePath(const std::string& path);

    private:
        std::string root;
        std::string directory;
        std::string filename;
        std::string extension;

        unsigned int deep_length;

        State path_state;
    };
}
WPE_NAMESPACE_END
#endif