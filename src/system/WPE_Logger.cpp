#include <system\WPE_Logger.h>

using namespace WPE_NAMESPACE::Log;

/*********************************************************************** */
GlobalLogger& GlobalLogger::GetInstance() {
    static GlobalLogger logger;
    return logger;
}

GlobalLogger::~GlobalLogger() {
    for(auto& slot: this->slots) {
        slot->Flush();
        slot.reset();
    }
}

void GlobalLogger::Log(const LogMessage& message) {
    for(auto& slot: this->slots) {
        slot->Write(message);
    }
}

void GlobalLogger::Flush() {
    for(auto& slot: this->slots) {
        slot->Flush();
    }
}

void GlobalLogger::AddLogSlot(std::unique_ptr<LogSlot>& slot) {
    this->slots.push_back(std::move(slot));
}

/*********************************************************************** */

const char* LogSlot::MessageType(LOG_LEVEL level) {
    static const char* message[] = {"MESSAGE", "WARNING", "ERROR"};

    switch(level) {
    case LOG_LEVEL_MESSAGE:
        return message[0];
    case LOG_LEVEL_WARNING:
        return message[1];
    case LOG_LEVEL_ERROR:
        return message[2];
    }
}

void ConsoleLogSlot::Write(const LogMessage& message) {
    // format output
    this->io.Write(Base::format("%s %s", MessageType(message.level), message.message));
}

void FileLogSlot::OpenLogFile(const std::string& filename) {
    if(!this->io.OpenFile(filename))
        throw std::runtime_error(Base::format("Couldn't open file: %s", filename));
}

void FileLogSlot::Write(const LogMessage& message) {
    this->io.Write(Base::format("%s %s", MessageType(message.level), message.message));
}

void FileLogSlot::Flush() {
    this->io.Flush();
}
