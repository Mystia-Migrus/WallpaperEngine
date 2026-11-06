#pragma once
#ifndef WPE_BASE_H
#define WPE_BASE_H

#include <string>
#include <vector>
#include <memory>
#include <array>
#include <stdexcept>

#define WPE_NAMESPACE WPE
#define WPE_NAMESPACE_BEGIN namespace WPE_NAMESPACE {
#define WPE_NAMESPACE_END }

#define WPE_DEFAULT_BUFFERSIZE 512
#define WPE_CONSOLEIO_DEFAULT_BUFFERSIZE WPE_DEFAULT_BUFFERSIZE

WPE_NAMESPACE_BEGIN
namespace Base {

    template<typename ...Args> 
    std::string format(const std::string& format, const Args&... args) {
        char buffer[WPE_DEFAULT_BUFFERSIZE] = {0};
        std::sprintf(buffer, format.c_str(), args...);
        
        return std::string(buffer);
    }

//------------------------------------------------
// IO System
    template<typename T>
    class IO {
    public:
        /// @brief IO Write
        /// @param data data
        /// @return If succeed, return true;
        virtual bool Write(const T& data) = 0;

        /// @brief IO Read
        /// @param buffer a memory 
        /// @return If success, return the byte of read; if not, return MAX_UINT;
        virtual unsigned int Read(T& buffer) = 0;
    };

    using StringIO = IO<std::string>;
    using ByteIO = IO<unsigned char>;
    
    class ConsoleIO: public StringIO {
    public:
        ConsoleIO() = default;

    public:
        virtual bool Write(const std::string& data);
        virtual unsigned int Read(std::string& buffer);
    
    private:
        
    };

    class FileIO: public StringIO {
    public:
        FileIO() = default; 


    public:
        bool OpenFile(const std::string& filename);

        virtual bool Write(const std::string& data);
        virtual unsigned int Read(std::string& buffer);

        void Flush();

        const std::string& GetFilename() const;
    private:
        std::string filename;
        bool status;

        std::array<char, WPE_CONSOLEIO_DEFAULT_BUFFERSIZE> buffer;
        unsigned int length = 0;
    };

    
};
WPE_NAMESPACE_END

#endif // WPE_BASE_H