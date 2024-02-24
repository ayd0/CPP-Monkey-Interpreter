#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct Lexer {
    std::string input;
    unsigned int position;
    unsigned int readPosition;
    unsigned char ch;

    Lexer(std::string inp)
        : input(inp) {}
};

void readChar(Lexer &l);
token::Token newToken(token::TokenType tokenType, unsigned char ch);
token::Token newToken(token::TokenType tokenType, std::string literal);
Lexer New(std::string input);
token::Token NextToken(Lexer &l);

#endif // LEXER_H

