#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <iostream>
#include <functional>
#include <map>

struct Parser {
    Lexer &l;
    std::vector<std::string> errors;
    token::Token curToken;
    token::Token peekToken;

    using prefixParseFn = std::function<ast::Expression*()>;
    using infixParseFn  = std::function<ast::Expression*(ast::Expression*)>;
    std::map<token::TokenType, prefixParseFn> prefixParseFns;
    std::map<token::TokenType, infixParseFn>  infixParseFns;
    
    enum class Order {
        LOWEST,
        EQUALS,
        LESSGREATER,
        SUM,
        PRODUCT,
        PREFIX,
        CALL
    };

    Parser(Lexer &l) 
        : l(l), curToken(l.NextToken()), peekToken(l.NextToken()) {
            registerPrefix(token::IDENT, [this]() -> ast::Expression* { return this->parseIdentifier(); });
            registerPrefix(token::INT,   [this]() -> ast::Expression* { return this->parseIntegerLiteral(); });
        }

    ast::Program ParseProgram();
    void checkParserErrors();
    void registerPrefix(token::TokenType tokenType, prefixParseFn fn);
    void registerInfix(token::TokenType tokenType, infixParseFn fn);

private:
    void nextToken();
    ast::Statement*           parseStatement();
    ast::Statement*           parseLetStatement();
    ast::Statement*           parseReturnStatement();
    ast::ExpressionStatement* parseExpressionStatement();
    ast::Expression*          parseExpression(Order precedence);
    ast::Expression*          parseIdentifier();
    ast::Expression*          parseIntegerLiteral();
    bool                      expectPeek(token::TokenType);
    bool                      curTokenIs(token::TokenType);
    bool                      peekTokenIs(token::TokenType);
    void                      peekError(token::TokenType);
    std::vector<std::string>  Errors();
};

#endif // PARSER_H
