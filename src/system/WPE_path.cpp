#include <system/WPE_path.h>
#include <algorithm>

using namespace WPE_NAMESPACE::Path;

#define IsBannedCode(c) (c == '"' || c == '|' || c == '<' || c == '>' || c == '*' || c == '?')
#define IsLetter(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

enum TokenType {
    TOKEN_TYPE_DRIVER,          // 盘符
    TOKEN_TYPE_SEPARATOR,       // 分隔符
    TOKEN_TYPE_COMPONENT,       // 路径
    TOKEN_TYPE_PARENT,          // ..
    TOKEN_TYPE_COMMA,           // .
    TOKEN_TYPE_ERROR            // 错误
};

const char* TokenStrings[] {
    "DRIVER",
    "SEPARATOR",
    "COMPONENT",
    "PARENT",
    "COMMA",
    "ERROR"
};

enum RootType {
    ROOT_TYPE_RELATIVE,
    ROOT_TYPE_DRIVE_ABSOLUTE
};

struct StringIndex {
    unsigned int index;
    unsigned int length;

    StringIndex(): index(0), length(0) {}
    StringIndex(unsigned int index, unsigned int length): index(index), length(length){}

    inline std::string GetString(const std::string& src) {
        return src.substr(index, length);
    }

};

struct AnalysisResult {
    RootType type;
    StringIndex driver;
    StringIndex directory;
    StringIndex filename;
};

struct AnalysisResult2 {
    std::string preprocessed_path;
    unsigned deep_length;
    AnalysisResult result;

    inline std::string GetDriver() {
        return result.driver.GetString(preprocessed_path);
    }

    inline std::string GetDirectory() {
        return result.directory.GetString(preprocessed_path);
    }

    inline std::string GetFilename() {
        return result.filename.GetString(preprocessed_path);
    }
};

struct Token {
    TokenType type;
    StringIndex index;

    Token(TokenType type, StringIndex index): type(type), index(index){}

    std::string Serialize(const std::string& src) {
        return WPE::Base::format("{%s, \"%s\"}", TokenStrings[type], src.substr(index.index, index.length).c_str());
    }
};

void Preprocessed(std::string& string) {
    // 预处理
    const char* blanks = " \t\n\r\f\v"; // 包含空格、制表符、换行符等
    string.erase(0, string.find_first_not_of(blanks)); // 去掉前导空白
    string.erase(string.find_last_not_of(blanks) + 1); // 去掉尾部空白
}

void EmitComponent(const std::string& path, unsigned int begin, unsigned int end, std::vector<Token>& tokens) {
    if(end <= begin) return;

    unsigned int length = end - begin;
    if(length == 1 && path[begin] == '.') {
        tokens.push_back(Token(TOKEN_TYPE_COMMA, {begin, 1}));
    }
    else if(length == 2 && path[begin] == '.' && path[begin + 1] == '.') {
        tokens.push_back(Token(TOKEN_TYPE_PARENT, {begin, 2}));
    }
    else {
        tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {begin, length}));
    }
}

// 不处理盘符相对路径
bool AnalyseToken(const std::string& preprocessed_path, std::vector<Token>& tokens) {
    unsigned int index = 0, index_buffer = 0;
    unsigned int length = preprocessed_path.size();
    
    if(length == 0)
        return false;
    
    if(length >= 2 && IsLetter(preprocessed_path[0]) && preprocessed_path[1] == ':') {
        if(length == 2) {
            tokens.push_back(Token(TOKEN_TYPE_DRIVER, {0, 2}));
            return true;
        }

        if(preprocessed_path[2] != '\\' && preprocessed_path[2] != '/') {
            return false;
        }

        tokens.push_back(Token(TOKEN_TYPE_DRIVER, {0, 3}));

        index = index_buffer = 3;
    }
    else if(preprocessed_path[0] == '/' || preprocessed_path[0] == '\\') {
        tokens.push_back(Token(TOKEN_TYPE_SEPARATOR, {0, 1}));
        index = index_buffer = 1;
    }
    else {
        index = index_buffer = 0;
    }

    char c;
    while(index < length) {
        c = preprocessed_path[index];
        switch (c)
        {
        case '\\':
        case '/':
            EmitComponent(preprocessed_path, index_buffer, index, tokens);
            tokens.push_back(Token(TOKEN_TYPE_SEPARATOR, {index, 1}));
            ++index;
            index_buffer = index;
            break;
        default:
            if(IsBannedCode(c) || c == ':')
                return false;
            else
                ++index;
            break;
        }
    }
    EmitComponent(preprocessed_path, index_buffer, index, tokens);

    return true;
} 

unsigned int TokensAnalyse(const std::vector<Token> tokens, AnalysisResult& result) {
    auto& itor = tokens.begin();
    unsigned int begin = 0, length = 0, deeplength = 0;

    if(itor->type == TOKEN_TYPE_DRIVER) {
        result.type = ROOT_TYPE_DRIVE_ABSOLUTE;
        result.driver = itor->index;
        begin = itor->index.length;
    }
    else if((itor->type == TOKEN_TYPE_SEPARATOR || itor->type == TOKEN_TYPE_COMPONENT) && itor + 1 != tokens.end()) {
        result.type = ROOT_TYPE_RELATIVE;
        begin = itor->index.index;
        length = itor->index.length;

        ++deeplength;
    }
    else if(itor->type == TOKEN_TYPE_SEPARATOR || itor->type == TOKEN_TYPE_COMPONENT) {
        result.type = ROOT_TYPE_RELATIVE;
        result.filename = itor->index;
        return 0;
    }

    ++itor;

    for(; itor != tokens.end(); ++itor) {
        switch(itor->type) {
        case TOKEN_TYPE_COMPONENT:
            if(itor + 1 == tokens.end()) {
                result.filename = itor->index;
                break;    
            }
            length += itor->index.length;
            break;
        case TOKEN_TYPE_SEPARATOR:
            ++deeplength;
            length += itor->index.length;
            break;
        }
    }

    result.directory = {begin, length};
    return deeplength;
}

void AnalysePath(const std::string& path, AnalysisResult2& result) {
    result.preprocessed_path = path;
    Preprocessed(result.preprocessed_path);

    std::vector<Token> tokens;
    if(AnalyseToken(result.preprocessed_path, tokens)) {
        result.deep_length = TokensAnalyse(tokens, result.result);
    }
}

std::string PathNormalize(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '/', '\\');
    return result;
}

//---------------------------------------------------------
// Path

Path::Path(const std::string &path): deep_length(0) {
    AnalysisResult2 result;

    AnalysePath(path, result);

    this->driver = result.GetDriver();
    this->directory = PathNormalize(result.GetDirectory());
    this->filename = result.GetFilename();
}

Path& Path::Append(const std::string& path) {
    AnalysisResult2 result;

    AnalysePath(path, result);

    this->directory.append(PathNormalize(result.GetDirectory()));

    return *this;
}

void Path::Clear() {
    this->driver.clear();
    this->directory.clear();
    this->filename.clear();
    deep_length = 0;
}

const std::string Path::Native() const {
    return this->driver + this->directory + this->filename;
}

const std::string& Path::Driver() const {
    return this->driver;
}

const std::string& Path::Directory() const {
    return this->directory;
}

const std::string& Path::Filename() const {
    return this->filename;
}

unsigned int Path::GetDeepLength() const {
    return this->deep_length;
}