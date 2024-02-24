#include "../../include/lexer.h"

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

    ++l.readPosition;
}

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
