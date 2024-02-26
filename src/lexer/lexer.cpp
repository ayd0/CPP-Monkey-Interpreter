#include "../../include/lexer.h"
#include <iostream>

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0;
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
}

unsigned char Lexer::peekChar() {
    if (readPosition >= input.length()) {
        return 0;
    } else {
        return input[readPosition];
    }
}

void Lexer::skipWhitespace() {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        readChar();
    }
}

bool isLetter(unsigned char ch);
bool isDigit(unsigned char ch);

token::Token Lexer::NextToken() {
    token::Token tok;

    skipWhitespace();

    switch (ch) {
        case '=' :
            if (peekChar() == '=') {
                unsigned char ch = ch;
                readChar();
                std::string literal = "==";
                tok = newToken(token::EQ, literal);
            } else {
                tok = newToken(token::ASSIGN, ch);
            }
            break;
        case ';' :
            tok = newToken(token::SEMICOLON, ch); break;
        case '(' :
            tok = newToken(token::LPAREN,    ch); break;
        case ')' :
            tok = newToken(token::RPAREN,    ch); break;
        case ',' :
            tok = newToken(token::COMMA,     ch); break;
        case '+' :
            tok = newToken(token::PLUS,      ch); break;
        case '-' :
            tok = newToken(token::MINUS,     ch); break;
        case '!' :
            if (peekChar() == '=') {
                unsigned char ch = ch;
                readChar();
                std::string literal = "!=";
                tok = newToken(token::NOT_EQ, literal);
            } else {
                tok = newToken(token::BANG, ch);
            }
            break;
        case '/' :
            tok = newToken(token::SLASH,    ch); break;
        case '*' :
            tok = newToken(token::ASTERISK, ch); break;
        case '<' :
            tok = newToken(token::LT,       ch); break;
        case '>' :
            tok = newToken(token::GT,       ch); break;
        case '{' :
            tok = newToken(token::LBRACE,   ch); break;
        case '}' :
            tok = newToken(token::RBRACE,   ch); break;
        case 0 :
            tok = newToken(token::EOF_T,    ""); break;
        default :
            if (isLetter(ch)) {
                tok.Literal = readIdentifier();
                tok.Type    = LookupIdent(tok.Literal);
                return tok;
            } else if (isDigit(ch)) {
                tok.Type = token::INT;
                tok.Literal = readNumber();
                return tok;
            } else {
                tok = newToken(token::ILLEGAL, ch);
            }
    }

    readChar();
    return tok;
}
std::string Lexer::readIdentifier() {
    unsigned int position = this->position;
    while (isLetter(ch)) {
        readChar();
    }

    return input.substr(position, this->position - position);
}

std::string Lexer::readNumber() {
    unsigned int position = this->position;
    while (isDigit(ch)) {
        readChar();
    }

    return input.substr(position, this->position - position);
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
