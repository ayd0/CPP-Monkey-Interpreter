#include "../../include/parser.h"
#include <iostream>
#include <stdexcept>

enum class Parser::Order {
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

std::map<token::TokenType, Parser::Order> Parser::precedences{
    {token::EQ,       Parser::Order::EQUALS},
    {token::NOT_EQ,   Parser::Order::EQUALS},
    {token::LT,       Parser::Order::LESSGREATER},
    {token::GT,       Parser::Order::LESSGREATER},
    {token::PLUS,     Parser::Order::SUM},
    {token::MINUS,    Parser::Order::SUM},
    {token::SLASH,    Parser::Order::PRODUCT},
    {token::ASTERISK, Parser::Order::PRODUCT},
};

void Parser::registerPrefix(token::TokenType tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(token::TokenType tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = fn;
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l.NextToken();
}

ast::Program Parser::ParseProgram() {
    ast::Program program = ast::Program();
    
    while (curToken.Type != token::EOF_T) {
        ast::Statement *stmt = parseStatement();
        if (stmt != nullptr) {
            program.Statements.push_back(stmt);
        }
        nextToken();
    }

    if (program.Statements.size() > 0) {
        program.isEmpty = false;
    }

    return program;
}

ast::Statement* Parser::parseStatement() {
    if (curToken.Type == token::LET) {
        return parseLetStatement();
    } else if (curToken.Type == token::RETURN) {
        return parseReturnStatement();   
    } else {
        return parseExpressionStatement();
    }
}

ast::Statement* Parser::parseLetStatement() {
    ast::LetStatement *stmt = new ast::LetStatement(curToken, peekToken);
    nextToken();

    if (!expectPeek(token::ASSIGN)) {
        return nullptr;
    }

    // TODO: skipping expressions until we encounter a semicolon
    while (!curTokenIs(token::SEMICOLON)) {
        nextToken();
    }

    return stmt;
}

ast::Statement* Parser::parseReturnStatement() {
    ast::ReturnStatement *stmt = new ast::ReturnStatement(curToken); 
   nextToken();

   // TODO: skipping expressions until we encounter a semicolon
   while (!curTokenIs(token::SEMICOLON)) {
       nextToken();
   }

   return stmt;
}

ast::ExpressionStatement* Parser::parseExpressionStatement() {
    ast::ExpressionStatement* stmt = new ast::ExpressionStatement(curToken);
    stmt->expression = parseExpression(Order::LOWEST);

    if (peekTokenIs(token::SEMICOLON)) {
        nextToken();
    }

    return stmt;
}

ast::Expression* Parser::parseExpression(Order precedence) {
    prefixParseFn prefix = prefixParseFns[curToken.Type];
    if (prefix == nullptr) {
        noPrefixParseFnError(curToken.Type);
        return nullptr;
    }
    ast::Expression* leftExp = prefix();

    while (!peekTokenIs(token::SEMICOLON) && precedence < peekPrecedence()) {
        infixParseFn infix = infixParseFns[peekToken.Type];
        if (infix == nullptr) {
            return leftExp;
        }

        nextToken();
        leftExp = infix(leftExp);
    }

    return leftExp;
}

ast::Expression* Parser::parseIdentifier() {
    return new ast::Identifier(curToken);
}

ast::Expression* Parser::parseIntegerLiteral() {
    ast::IntegerLiteral* ilit = new ast::IntegerLiteral(curToken);
    int64_t value;

    try {
        value = std::stoll(curToken.Literal);
    } catch (const std::invalid_argument& e) {
        std::string err = "parseIntegerLiteral(): invalid_argument: " + std::string(e.what());
        errors.push_back(err);
        return nullptr;
    } catch (const std::out_of_range& e) {
        std::string err = "parseIntegerLiteral(): out_of_range: " + std::string(e.what());
        errors.push_back(err);
        return nullptr;
    }

    ilit->Value = value;

    return ilit;
}

ast::Expression* Parser::parsePrefixExpression() {
    ast::PrefixExpression* pexpr = new ast::PrefixExpression(curToken);

    nextToken();
    pexpr->Right = parseExpression(Order::PREFIX);

    return pexpr;
}

ast::Expression* Parser::parseInfixExpression(ast::Expression* left) {
    ast::InfixExpression* iexpr = new ast::InfixExpression(curToken, left);

    Order precedence = curPrecedence();
    nextToken();
    iexpr->Right = parseExpression(precedence);

    return iexpr;
}

bool Parser::curTokenIs(token::TokenType t) {
   return curToken.Type == t; 
}

bool Parser::peekTokenIs(token::TokenType t) {
    return peekToken.Type == t;
}

Parser::Order Parser::curPrecedence() {
    auto it = precedences.find(curToken.Type);
    if (it != precedences.end()) {
        return it->second;
    }

    return Order::LOWEST;
}

Parser::Order Parser::peekPrecedence() {
    auto it = precedences.find(peekToken.Type);
    if (it != precedences.end()) {
        return it->second;
    }

    return Order::LOWEST;
}

bool Parser::expectPeek(token::TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::vector<std::string> Parser::Errors() {
    return errors;
}

void Parser::peekError(token::TokenType t){
    errors.push_back("expected next token to be " + t + 
            ", got " + peekToken.Type + " instead");
}
    
void Parser::noPrefixParseFnError(token::TokenType t) {
    errors.push_back("no prefix parse function for " + t + " found");
}

void Parser::checkParserErrors() {
    std::vector<std::string> errors = Errors();
    
    if (errors.size() == 0) {
        return;
    }

    std::cerr << "ERROR::PARSER: Parser has errors: (" << errors.size() << ")" << std::endl;
    for (std::string err : errors) {
        std::cerr << "ERROR::PARSER: " << err << std::endl;
    }

    return;
}
