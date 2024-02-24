#include "../../include/lexer.h"

void Lexer::readChar() {
    if (this->readPosition >= this->input.length()) {
        this->ch = 0;
    } else {
        this->ch = this->input[this->readPosition];
    }
    this->position = this->readPosition;
    this->readPosition += 1;
}

unsigned char Lexer::peekChar() {
    if (this->readPosition >= this->input.length()) {
        return 0;
    } else {
        return this->input[this->readPosition];
    }
}

void Lexer::skipWhitespace() {
    while (this->ch == ' ' || this->ch == '\t' || this->ch == '\n' || this->ch == '\r') {
        this->readChar();
    }
}

bool isLetter(unsigned char ch);
bool isDigit(unsigned char ch);

token::Token Lexer::NextToken() {
    token::Token tok;

    this->skipWhitespace();

    switch (this->ch) {
        case '=' :
            if (this->peekChar() == '=') {
                unsigned char ch = this->ch;
                this->readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = newToken(token::EQ, literal);
            } else {
                tok = newToken(token::ASSIGN, this->ch);
            }
            break;
        case ';' :
            tok = newToken(token::SEMICOLON, this->ch);  break;
        case '(' :
            tok = newToken(token::LPAREN,    this->ch);  break;
        case ')' :
            tok = newToken(token::RPAREN,    this->ch);  break;
        case ',' :
            tok = newToken(token::COMMA,     this->ch);  break;
        case '+' :
            tok = newToken(token::PLUS,      this->ch);  break;
        case '-' :
            tok = newToken(token::MINUS,     this->ch);  break;
        case '!' :
            if (this->peekChar() == '=') {
                unsigned char ch = this->ch;
                this->readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = newToken(token::NOT_EQ, literal);
            } else {
                tok = newToken(token::BANG, this->ch);
            }
            break;
        case '/' :
            tok = newToken(token::SLASH,     this->ch);  break;
        case '*' :
            tok = newToken(token::ASTERISK,  this->ch);  break;
        case '<' :
            tok = newToken(token::LT,        this->ch);  break;
        case '>' :
            tok = newToken(token::GT,        this->ch);  break;
        case '{' :
            tok = newToken(token::LBRACE,    this->ch);  break;
        case '}' :
            tok = newToken(token::RBRACE,    this->ch);  break;
        case 0 :
            tok = newToken(token::EOF_T,     "");    break;
        default :
            if (isLetter(this->ch)) {
                tok.Literal = this->readIdentifier();
                tok.Type    = LookupIdent(tok.Literal);
                return tok;
            } else if (isDigit(this->ch)) {
                tok.Type = token::INT;
                tok.Literal = this->readNumber();
                return tok;
            } else {
                tok = newToken(token::ILLEGAL, this->ch);
            }
    }

    this->readChar();
    return tok;
}
std::string Lexer::readIdentifier() {
    unsigned int position = this->position;
    while (isLetter(this->ch)) {
        this->readChar();
    }

    return this->input.substr(position, this->position - position);
}

std::string Lexer::readNumber() {
    unsigned int position = this->position;
    while (isDigit(this->ch)) {
        this->readChar();
    }

    return this->input.substr(position, this->position - position);
}

token::Token newToken(token::TokenType tokenType, unsigned char ch) {
    return token::Token{tokenType, std::string(1, ch)};
}

token::Token newToken(token::TokenType tokenType, std::string literal) {
    return token::Token{tokenType, literal};
}


bool isLetter(unsigned char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool isDigit(unsigned char ch) {
    return '0' <= ch && ch <= '9';
}
