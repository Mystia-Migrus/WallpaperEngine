#include <system/WPE_base.h>

using namespace WPE_NAMESPACE::Base;

#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>

// ----------------------------------------------------
// IO System
bool ConsoleIO::Write(const std::string& data) {
    std::cout << data.data() << std::endl;
    return true;
}

unsigned int ConsoleIO::Read(std::string& buffer) {
    std::cin >> buffer;
    return buffer.size();       // ?
}

bool FileIO::OpenFile(const std::string& filename) {
    std::ifstream file(filename);
    
    (this->status = file.good()) == true? this->filename = filename: 0;
    
    return this->status;
}

bool FileIO::Write(const std::string& data) {
    if(status) {
        unsigned new_length = length + data.size();
        if(new_length > this->buffer.size())
            Flush();

        std::copy(data.begin(), data.end(), this->buffer.begin() + length + 1);
        length += data.size();
        return true;
    }
    
    return false;
}

unsigned int FileIO::Read(std::string& buffer) {
    if(status){
        std::stringstream ss;
        std::ifstream file(this->filename);
        
        ss << file.rdbuf();
        file.close();
        buffer = ss.str();
        
        return buffer.size();   // ?
    }

    return UINT_MAX;
}

void FileIO::Flush() {
    if(this->status){
        std::ofstream file(this->filename);
        file.write(this->buffer.data(), this->buffer.size());
        file.close();

        this->buffer.fill('0');
        length = 0;
    }
}

const std::string& FileIO::GetFilename() const {
    return this->filename;
}