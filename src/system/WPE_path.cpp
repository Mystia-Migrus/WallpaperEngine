#include <system/WPE_path.h>

using namespace WPE_NAMESPACE::Path;

#define IsBannedCode(c) (c == '"' || c == '|' || c == '<' || c == '>' || c == '*' || c == '?')
#define IsLetter(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

enum TokenType {
    TOKEN_TYPE_DRIVER,          // 盘符
    TOKEN_TYPE_ROOT_RELATIVE,   // 根相对路径
    TOKEN_TYPE_SEPARATOR,       // 分隔符
    TOKEN_TYPE_COMPONENT,       // 路径
    TOKEN_TYPE_PARENT,          // ..
    TOKEN_TYPE_COMMA,           // .
    TOKEN_TYPE_ERROR            // 错误
};

const char* TokenStrings[] {
    "TOKEN_TYPE_DRIVER",
    "TOKEN_TYPE_ROOT_RELATIVE",
    "TOKEN_TYPE_SEPARATOR",
    "TOKEN_TYPE_COMPONENT",
    "TOKEN_TYPE_PARENT",
    "TOKEN_TYPE_COMMA",
    "TOKEN_TYPE_ERROR"
};

enum RootType {
    ROOT_TYPE_RELATIVE,
    ROOT_TYPE_DRIVE_ABSOLUTE,
    ROOT_TYPE_ROOT_RELATIVE
};

enum AnalysisType {
    ANALYSIS_TYPE_ROOT,
    ANALYSIS_TYPE_NEXT,
    ANALYSIS_TYPE_COMPONENT,
    ANALYSIS_TYPE_OVER,
    ANALYSIS_TYPE_ERROR
};

struct StringIndex {
    unsigned int index;
    unsigned int length;
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

// 不处理根相对路径
bool AnalyseToken(const std::string& preprocessed_path, std::vector<Token>& tokens) {
    unsigned int index = 0, index_buffer = 0;
    unsigned int length = preprocessed_path.size();
    AnalysisType state = ANALYSIS_TYPE_ROOT;

    // 空字符串自动跳过判断, 返回 true
    while(index < length && state == ANALYSIS_TYPE_ROOT) {
        switch(preprocessed_path[index]) {
        // 盘符
        case ':': 
            // 确保当前 index 为第二个字符, 并且第一个字符是字母, 无意义, 退出分析
            if((index != 1 || !IsLetter(preprocessed_path[0]))) {
                state = ANALYSIS_TYPE_ERROR;
                break;
            } 
            // 若字符串仅有 2 个字符, 即 [盘符]:
            if(length == 2) {
                tokens.push_back(Token(TOKEN_TYPE_DRIVER, {0, 2}));
                state = ANALYSIS_TYPE_OVER;
                break;
            }
            
            // 字符串一定大于两个字符, 不用担心溢出
            switch(preprocessed_path[++index]) {
            case '\\':
                tokens.push_back(Token(TOKEN_TYPE_DRIVER, {0, 3}));
                state = ANALYSIS_TYPE_NEXT;    // 完成盘符解析, 准备进入路径解析
                index_buffer = index + 1;
                break;

            // 若第三个字符为 '/'或其它字符, 无意义, 直接退出分析
            case '/':
            default:
                state = ANALYSIS_TYPE_ERROR;
                break;
            }

            ++index;
            break;
        // 根相对路径或相对路径
        case '\\':
        case '/':
            // 进入其它状态
            if(index == 0) {
                tokens.push_back(Token(TOKEN_TYPE_SEPARATOR, {0, 1}));
                index_buffer = 1;
                state = ANALYSIS_TYPE_NEXT;
            } 
            else {
                tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {0, index}));
                tokens.push_back(Token(TOKEN_TYPE_SEPARATOR, {index, 1}));
                index_buffer = index + 1;   // 若 index + 1 >= length, 该 index_buffer 不会被使用到
                state = ANALYSIS_TYPE_NEXT;
            }

            ++index;    // 防止重复分析
            break;
        // 可能为相对路径
        case '.':
            ++index;
            
            if(index < length && preprocessed_path[index] == '.') {
                tokens.push_back(Token(TOKEN_TYPE_PARENT, {index - 1, 2}));
                ++index;    // 防止重复分析
                index_buffer = index;
                break;
            } 
            else {
                tokens.push_back(Token(TOKEN_TYPE_COMMA, {index - 1, 1}));
                index_buffer = index;
            }
            break;
        default:
            // 无意义字符, 退出分析
            if(IsBannedCode(preprocessed_path[index])) {
                state = ANALYSIS_TYPE_ERROR;
            } 
            // 全为合法字符, 但无分隔符, 同时索引越界, 这里判断为文件名
            else if(index >= length) {
                tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {0, index + 1}));
                state = ANALYSIS_TYPE_OVER;
                break;
            }

            ++index;
        }
        
        if(state == ANALYSIS_TYPE_OVER) {
            return true;
        } 
        else if(state == ANALYSIS_TYPE_ERROR) {
            return false;
        }  
    }

    // ANALYSIS_STATE_COMPONENT
    if(index < length) 
        state = ANALYSIS_TYPE_COMPONENT;

    while(index < length && state == ANALYSIS_TYPE_COMPONENT) {
        switch(preprocessed_path[index]) {
        // 出现路径分隔
        case '\\':
        case '/':
            tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {index_buffer, index - index_buffer}));
            tokens.push_back(Token(TOKEN_TYPE_SEPARATOR, {index, 1}));
            index_buffer = index + 1;

            ++index;
            break;
        case '.':
            tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {index_buffer, index - index_buffer}));
            ++index;
            if(index < length && preprocessed_path[index] == '.') {
                tokens.push_back(Token(TOKEN_TYPE_PARENT, {index - 1, 2}));
                ++index;
                index_buffer = index;
                break;
            }
            else {
                tokens.push_back(Token(TOKEN_TYPE_COMMA, {index - 1, 1}));
                index_buffer = index;  
            }
            // 当前字符留到下一轮处理
            break;
        default:
            if(IsBannedCode(preprocessed_path[index]) || preprocessed_path[index] == ':') {
                // 出现错误字符, 路径无意义
                state = ANALYSIS_TYPE_ERROR;
                break;
            }
            ++index;
        }

        if(state == ANALYSIS_TYPE_OVER) {
            break;
        } else if(state == ANALYSIS_TYPE_ERROR) {
            return false;
        }

    }

    if(state == ANALYSIS_TYPE_COMPONENT && index >= length && index_buffer < length) {
        tokens.push_back(Token(TOKEN_TYPE_COMPONENT, {index_buffer, length - index_buffer}));
    }
    return true;
} 

void Path::AnalysePath(const std::string &path) {
    std::string buffer = path;

    Preprocessed(buffer);
    // 词元分析
    std::vector<Token> tokens;
    AnalyseToken(buffer, tokens);
}

Path::Path(const std::string &path): deep_length(0), path_state(PATH_STATE_SYNTAX_ERROR) {
    AnalysePath(path);
}
