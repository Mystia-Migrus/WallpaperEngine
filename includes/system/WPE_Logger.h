#pragma once
#ifndef WPE_LOGGER_H
#define WPE_LOGGER_H
#include <system/WPE_base.h>

WPE_NAMESPACE_BEGIN
namespace Log {
    enum LOG_LEVEL {
        LOG_LEVEL_MESSAGE,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_ERROR
    };

    struct LogMessage {
        LOG_LEVEL level;
        std::string message;

        LogMessage(const std::string& message, LOG_LEVEL level = LOG_LEVEL_MESSAGE): level(level), message(message) {}
    };

    
    class LogSlot {
    public:
        virtual ~LogSlot() = default;

        virtual void Write(const LogMessage&) = 0;
        virtual void Flush() {}

    protected: 
        static const char* MessageType(LOG_LEVEL level);
    };
    
    class ConsoleLogSlot: public LogSlot {
    public:
        virtual void Write(const LogMessage& message);
         
    private:
        
        Base::ConsoleIO io;
    };

    class FileLogSlot: public LogSlot {
    public:
        void OpenLogFile(const std::string& log_filename);
        virtual void Write(const LogMessage& message);

        void Flush();

    private:
        Base::FileIO io;
    };

// --------------------------------------------------------
    class GlobalLogger {
    public:
        ~GlobalLogger();
        
        GlobalLogger& GetInstance() const;

    public:
        void Log(const LogMessage& message);

        void AddLogSlot(std::unique_ptr<LogSlot>& slot);
    private:
        
        std::vector<std::unique_ptr<LogSlot>> slots;
    };


}
WPE_NAMESPACE_END
#endif