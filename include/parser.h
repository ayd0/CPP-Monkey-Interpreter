#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <iostream>
#include <functional>
#include <map>
#include <vector>

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
            registerPrefix(token::IDENT,    [this]() -> ast::Expression* { return this->parseIdentifier(); });
            registerPrefix(token::INT,      [this]() -> ast::Expression* { return this->parseIntegerLiteral(); });
            registerPrefix(token::STRING,   [this]() -> ast::Expression* { return this->parseStringLiteral(); });
            registerPrefix(token::LBRACKET, [this]() -> ast::Expression* { return this->parseArrayLiteral(); });
            registerPrefix(token::LBRACE,   [this]() -> ast::Expression* { return this->parseHashLiteral(); });
            registerPrefix(token::FALSE,    [this]() -> ast::Expression* { return this->parseBoolean(); });
            registerPrefix(token::TRUE,     [this]() -> ast::Expression* { return this->parseBoolean(); });
            registerPrefix(token::BANG,     [this]() -> ast::Expression* { return this->parsePrefixExpression(); });
            registerPrefix(token::MINUS,    [this]() -> ast::Expression* { return this->parsePrefixExpression(); });
            registerPrefix(token::LPAREN,   [this]() -> ast::Expression* { return this->parseGroupedExpression(); });
            registerPrefix(token::IF,       [this]() -> ast::Expression* { return this->parseIfExpression(); });
            registerPrefix(token::FUNCTION, [this]() -> ast::Expression* { return this->parseFunctionLiteral(); });

            registerInfix(token::PLUS,      [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::MINUS,     [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::SLASH,     [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::ASTERISK,  [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::EQ,        [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::NOT_EQ,    [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::LT,        [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::GT,        [this](ast::Expression* left) ->     ast::Expression* { return this->parseInfixExpression(left); });
            registerInfix(token::LBRACKET,  [this](ast::Expression* left) ->     ast::Expression* { return this->parseIndexExpression(left); });
            registerInfix(token::LPAREN,    [this](ast::Expression* function) -> ast::Expression* { return this->parseCallExpression(function); });
        }

    ast::Program ParseProgram();
    std::vector<std::string> Errors();
    void checkParserErrors();
    void registerPrefix(token::TokenType tokenType, prefixParseFn fn);
    void registerInfix(token::TokenType tokenType, infixParseFn fn);

private:
    void nextToken();
    ast::Statement*                parseStatement();
    ast::Statement*                parseLetStatement();
    ast::Statement*                parseReturnStatement();
    ast::ExpressionStatement*      parseExpressionStatement();
    ast::Expression*               parseExpression(Order precedence);
    ast::Expression*               parseIdentifier();
    ast::Expression*               parseIntegerLiteral();
    ast::Expression*               parseStringLiteral();
    ast::Expression*               parseArrayLiteral();
    ast::Expression*               parseBoolean();
    ast::Expression*               parsePrefixExpression();
    ast::Expression*               parseInfixExpression(ast::Expression*);
    ast::Expression*               parseGroupedExpression();
    ast::Expression*               parseIfExpression();
    ast::Expression*               parseFunctionLiteral();
    ast::Expression*               parseAssignExpression(ast::Expression*);
    ast::Expression*               parseCallExpression(ast::Expression*);
    ast::Expression*               parseIndexExpression(ast::Expression*);
    ast::Expression*               parseHashLiteral();
    std::vector<ast::Identifier*>  parseFunctionParameters();
    std::vector<ast::Expression*>  parseCallArguments();
    ast::BlockStatement*           parseBlockStatement();
    std::vector<ast::Expression*>  parseExpressionList(token::TokenType end);
    bool                           expectPeek(token::TokenType);
    bool                           curTokenIs(token::TokenType);
    bool                           peekTokenIs(token::TokenType);
    Order                          curPrecedence();
    Order                          peekPrecedence();
    void                           peekError(token::TokenType);
    void                           noPrefixParseFnError(token::TokenType);
};

#endif // PARSER_H
