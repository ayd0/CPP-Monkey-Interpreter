#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace token {
    typedef std::string TokenType;

    struct Token {
        TokenType Type;
        std::string Literal;
    };

    // Special types
    const TokenType ILLEGAL   = "ILLEGAL";
    const TokenType EOF_T  = "EOF";
    // Identifiers + literals
    const TokenType IDENT     = "IDENT";
    const TokenType INT       = "INT";
    // Operators
    const TokenType ASSIGN    = "=";
    const TokenType PLUS      = "+";
    // Delimiters
    const TokenType COMMA     = ",";
    const TokenType SEMICOLON = ";";
    const TokenType LPAREN    = "(";
    const TokenType RPAREN    = ")";
    const TokenType LBRACE    = "{";
    const TokenType RBRACE    = "}";
    // Keywords
    const TokenType FUNCTION  = "FUNCTION";
    const TokenType LET       = "LET";
}

#endif // TOKEN_H
