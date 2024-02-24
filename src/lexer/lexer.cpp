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
    l.readPosition += 1;
}

void skipWhitespace(Lexer &l);
std::string readIdentifier(Lexer &l);
std::string readNumber(Lexer &l);
bool isLetter(unsigned char ch);
bool isDigit(unsigned char ch);

token::Token NextToken(Lexer &l) {
    token::Token tok;

    skipWhitespace(l);

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
            tok = newToken(token::EOF_T,     ""); break;
        default :
            if (isLetter(l.ch)) {
                tok.Literal = readIdentifier(l);
                tok.Type    = LookupIdent(tok.Literal);
                return tok;
            } else if (isDigit(l.ch)) {
                tok.Type = token::INT;
                tok.Literal = readNumber(l);
                return tok;
            } else {
                tok = newToken(token::ILLEGAL, l.ch);
            }
    }

    readChar(l);
    return tok;
}

void skipWhitespace(Lexer &l) {
    while (l.ch == ' ' || l.ch == '\t' || l.ch == '\n' || l.ch == '\r') {
        readChar(l);
    }
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

    return l.input.substr(position, l.position - position);
}

std::string readNumber(Lexer &l) {
    unsigned int position = l.position;
    while (isDigit(l.ch)) {
        readChar(l);
    }

    return l.input.substr(position, l.position - position);
}

bool isLetter(unsigned char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool isDigit(unsigned char ch) {
    return '0' <= ch && ch <= '9';
}
