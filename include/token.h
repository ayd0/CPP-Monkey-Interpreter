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
    const TokenType EOF_T     = "EOF";
    // Identifiers + literals
    const TokenType IDENT     = "IDENT";
    const TokenType INT       = "INT";
    const TokenType STRING    = "STRING";
    // Operators
    const TokenType ASSIGN    = "=";
    const TokenType PLUS      = "+";
    const TokenType MINUS     = "-";
    const TokenType BANG      = "!";
    const TokenType ASTERISK  = "*";
    const TokenType SLASH     = "/";
    const TokenType LT        = "<";
    const TokenType GT        = ">";
    const TokenType EQ        = "==";
    const TokenType NOT_EQ    = "!=";
    // Delimiters
    const TokenType COMMA     = ",";
    const TokenType SEMICOLON = ";";
    const TokenType LPAREN    = "(";
    const TokenType RPAREN    = ")";
    const TokenType LBRACKET  = "[";
    const TokenType RBRACKET  = "]";
    const TokenType LBRACE    = "{";
    const TokenType RBRACE    = "}";
    // Keywords
    const TokenType FUNCTION  = "FUNCTION";
    const TokenType LET       = "LET";
    const TokenType TRUE      = "TRUE";
    const TokenType FALSE     = "FALSE";
    const TokenType IF        = "IF";
    const TokenType ELSE      = "ELSE";
    const TokenType RETURN    = "RETURN";
}

token::TokenType LookupIdent(std::string ident);
token::Token newToken(token::TokenType tokenType, unsigned char ch);
token::Token newToken(token::TokenType tokenType, std::string literal);

#endif // TOKEN_H
