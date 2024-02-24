#include "../../include/lexer.h"
#include <iostream>

Lexer New(std::string input) {
    Lexer l(input);
    readChar(l);
    return l;
}

void readChar(Lexer &l) {
    if (l.readPosition >= l.input.length()) {
        l.ch = 0;
    } else {
        l.ch = l.input[l.readPosition];
    }
    l.position = l.readPosition;
    ++l.readPosition;
}

std::string readIdentifier(Lexer &l);
bool isLetter(unsigned char ch);

token::Token NextToken(Lexer &l) {
    token::Token tok;

    switch (l.ch) {
        case '=' :
            tok = newToken(token::ASSIGN,    l.ch);  break;
        case ';' :
            tok = newToken(token::SEMICOLON, l.ch);  break;
        case '(' :
            tok = newToken(token::LPAREN,    l.ch);  break;
        case ')' :
            tok = newToken(token::RPAREN,    l.ch);  break;
        case ',' :
            tok = newToken(token::COMMA,     l.ch);  break;
        case '+' :
            tok = newToken(token::PLUS,      l.ch);  break;
        case '{' :
            tok = newToken(token::LBRACE,    l.ch);  break;
        case '}' :
            tok = newToken(token::RBRACE,    l.ch);  break;
        case 0 :
            tok = newToken(token::EOF_T,     "EOF"); break;
        default :
            if (isLetter(l.ch)) {
                tok.Literal = readIdentifier(l);
                tok.Type    = LookupIdent(tok.Literal);
                return tok;
            } else {
                tok = newToken(token::ILLEGAL, l.ch);
            }
    }

    readChar(l);
    return tok;
}

token::Token newToken(token::TokenType tokenType, unsigned char ch) {
    return token::Token{tokenType, std::string(1, ch)};
}

token::Token newToken(token::TokenType tokenType, std::string literal) {
    return token::Token{tokenType, literal};
}

std::string readIdentifier(Lexer &l) {
    unsigned int position = l.position;
    while (isLetter(l.ch)) {
        readChar(l);
    }

    std::cout << "Initial char: `" << l.ch << "` (int value: " << static_cast<int>(l.ch) << ")" << std::endl;
    std::cout << "Position: `" << position << "` l.position: `" << static_cast<int>(l.ch) << "`" << std::endl;
    std::cout << "***************** `" << l.input.substr(position, l.position - position) << "`" << std::endl;

    return l.input.substr(position, l.position - position);
}

bool isLetter(unsigned char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}
