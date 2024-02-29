#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct Lexer {
    std::string input;
    unsigned int position;
    unsigned int readPosition;
    unsigned char ch;

    Lexer(std::string inp)
        : input(inp), position(0), readPosition(0), ch('\0') {
            readChar();
        }

    void readChar();
    unsigned char peekChar();
    token::Token NextToken();

private: 
    void skipWhitespace();
    std::string readIdentifier();
    std::string readNumber();
    std::string readString();
};

#endif // LEXER_H

