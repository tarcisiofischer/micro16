#ifndef MICRO16_LEXER_HPP
#define MICRO16_LEXER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

enum class TokenType {
    IDENTIFIER,
    INTEGER,
    ENDLINE
};

inline std::string token_type_as_str(TokenType t)
{
    switch(t) {
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::INTEGER:
            return "INTEGER";
        case TokenType::ENDLINE:
            return "ENDLINE";
    }
    return "<?>";
}

struct Token {
    int line;
    int col;
    TokenType type;
    std::string data = "";
};

class Lexer {
public:
    static std::vector<Token> tokens_from_file(std::string const &input_file);

private:
    explicit Lexer(std::istream &ss);
    std::vector<Token> generate_tokens();
    void identifier(std::string &identifier_data);
    void number(std::string &identifier_data);
    void comment();
    char peek_next();
    char next();

    std::istream& ss;
    std::vector<Token> tokens;
    int line = 1;
    int startcol = 1;
    int col = 0;
};

#endif
