#include <assembler/lexer.hpp>

bool is_alpha(char c)
{
    return ((c >= 'a') && (c <= 'z')) ||
           ((c >= 'A') && (c <= 'Z')) || (c == '_');
}

bool is_digit(char c)
{
    return (c >= '0') && (c <= '9');
}

bool is_binary_digit(char c)
{
    return (c == '0') || (c == '1');
}

bool is_hex_digit(char c)
{
    return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r';
}

bool is_linebreak(char c)
{
    return c == '\n';
}

std::vector<Token> Lexer::tokens_from_file(std::string const &input_file)
{
    auto file_contents = std::ifstream{input_file, std::ios::in};
    if (file_contents.fail()) {
        throw std::runtime_error("Could not open file " + input_file);
    }

    auto lex = Lexer{file_contents};
    return lex.generate_tokens();
}

Lexer::Lexer(std::istream &ss) : ss(ss)
{
}

std::vector<Token> Lexer::generate_tokens()
{
    this->ss.seekg(0);
    for (auto i = 0; !this->ss.eof(); ++i) {
        auto c = this->next();
        this->startcol = this->col;
        if (c == EOF) {
            break;
        } else if (is_whitespace(c)) {
            if (c == ' ') {
                this->col += 1;
            } else if (c == '\t') {
                this->col += 4;
            }
        } else if (is_linebreak(c)) {
            this->tokens.push_back(Token{this->line, this->startcol, TokenType::ENDLINE});
            this->line += 1;
            this->col = 1;
        } else if (is_alpha(c)) {
            auto data = std::string{c};
            this->identifier(data);
        } else if (is_digit(c)) {
            auto data = std::string{c};
            this->number(data);
        } else if (c == '/') {
            this->comment();
        } else {
            throw std::runtime_error("Unexpected '" + std::string{c} + "' at line " + std::to_string(this->line));
        }
    }
    return this->tokens;
}

void Lexer::identifier(std::string &data)
{
    while (is_alpha_numeric(this->peek_next())) {
        data += this->next();
    }
    this->tokens.push_back(Token{this->line, this->startcol, TokenType::IDENTIFIER, data});
}

void Lexer::number(std::string &data)
{
    if (data[0] == '0') {
        if (this->peek_next() == 'x') {
            // Hex
            data += this->next(); // consume 'x'
            while (is_hex_digit(this->peek_next())) {
                data += this->next();
            }
        } else if (this->peek_next() == 'b') {
            // Binary
            data += this->next(); // consume 'b'
            while (is_binary_digit(this->peek_next())) {
                data += this->next();
            }
        } else {
            // Decimal
            while (is_digit(this->peek_next())) {
                data += this->next();
            }
        }
    }
    this->tokens.push_back(Token{this->line, this->startcol, TokenType::INTEGER, data});
}

void Lexer::comment()
{
    auto c = this->next();
    if (c == '/') {
        // Single line comment
        while (!is_linebreak(this->next())) {
            continue;
        }
    } else if (c == '*') {
        // Comment block
        c = this->next();
        while (!(c == '*' && this->peek_next() == '/')) {
            c = this->next();
            if (is_linebreak(c)) {
                this->line += 1;
                this->col = 0;
            }
            continue;
        }
        (void) this->next(); // consume last '/'
    } else {
        throw std::runtime_error("Unexpected '" + std::string{c} + "' at line " + std::to_string(this->line) + " (Was expecting a comment)");
    }
}

char Lexer::peek_next()
{
    return static_cast<char>(this->ss.peek());
}

char Lexer::next()
{
    this->col += 1;
    return static_cast<char>(this->ss.get());
}
