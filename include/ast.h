#ifndef AST_H
#define AST_H

#include "token.h"

#include <string>
#include <vector>

class Node {
public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
};

class Statement : public Node {
public:
    virtual void statementNode() = 0;
};

class Expression : public Node {
public:
    virtual void expressionNode() = 0;
};

struct Program {
    std::vector<Statement*> Statements;
    bool isEmpty = true;

    ~Program() {
        for (Statement* stmt : Statements) {
            delete stmt;
        }
        Statements.clear();
    }

    std::string TokenLiteral() {
        if (!Statements.empty()) {
            return Statements[0]->TokenLiteral();
        } else {
            return "";
        }
    }
};

struct Identifier : public Expression {
    token::Token Token;
    std::string Value;

    Identifier(token::Token token) : Token(token), Value(token.Literal) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
};

struct LetStatement : public Statement {
    token::Token Token;
    Identifier* Name;
    Expression* Value;

    LetStatement(token::Token token, token::Token nextToken) : Token(token), Value(nullptr) {
        Name = new Identifier(nextToken);
    }
    ~LetStatement() { 
        delete Value; 
        delete Name; 
    }

    void statementNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
};

#endif // AST_H
