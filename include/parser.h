#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <iostream>

struct Parser {
    Lexer &l;
    token::Token curToken;
    token::Token peekToken;
    std::vector<std::string> errors;

    Parser(Lexer &l) 
        : l(l), curToken(l.NextToken()), peekToken(l.NextToken()) {}

    Program ParseProgram();
    void checkParserErrors();

private:
    void nextToken();
    Statement* parseStatement();
    Statement* parseLetStatement();
    Statement* parseReturnStatement();
    bool       expectPeek(token::TokenType);
    bool       curTokenIs(token::TokenType);
    bool       peekTokenIs(token::TokenType);
    void       peekError(token::TokenType);
    std::vector<std::string> Errors();
};

#endif // PARSER_H
