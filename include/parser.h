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

    enum class Order;
    static std::map<token::TokenType, Order> precedences;

    Parser(Lexer &l) 
        : l(l), curToken(l.NextToken()), peekToken(l.NextToken()) {
            registerPrefix(token::IDENT,   [this]() -> ast::Expression* { return this->parseIdentifier(); });
            registerPrefix(token::INT,     [this]() -> ast::Expression* { return this->parseIntegerLiteral(); });
            registerPrefix(token::FALSE,   [this]() -> ast::Expression* { return this->parseBoolean(); });
            registerPrefix(token::TRUE,    [this]() -> ast::Expression* { return this->parseBoolean(); });
            registerPrefix(token::BANG,    [this]() -> ast::Expression* { return this->parsePrefixExpression(); });
            registerPrefix(token::MINUS,   [this]() -> ast::Expression* { return this->parsePrefixExpression(); });
            registerPrefix(token::LPAREN,  [this]() -> ast::Expression* { return this->parseGroupedExpression(); });

            registerInfix(token::PLUS,     [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::MINUS,    [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::SLASH,    [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::ASTERISK, [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::EQ,       [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::NOT_EQ,   [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::LT,       [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::GT,       [this](ast::Expression* left) -> ast::Expression* { return this->parseInfixExpression(left); });
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
    ast::Expression*          parseBoolean();
    ast::Expression*          parsePrefixExpression();
    ast::Expression*          parseInfixExpression(ast::Expression*);
    ast::Expression*          parseGroupedExpression();
    bool                      expectPeek(token::TokenType);
    bool                      curTokenIs(token::TokenType);
    bool                      peekTokenIs(token::TokenType);
    Order                     curPrecedence();
    Order                     peekPrecedence();
    void                      peekError(token::TokenType);
    void                      noPrefixParseFnError(token::TokenType);
    std::vector<std::string>  Errors();
};

#endif // PARSER_H
