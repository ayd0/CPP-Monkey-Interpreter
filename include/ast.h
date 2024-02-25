#ifndef AST_H
#define AST_H

#include "token.h"

#include <string>
#include <sstream>
#include <vector>

class Node {
public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
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

    Program() {}
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

    std::string String() {
        std::stringstream out;
        for (Statement* stmt : Statements) {
            out << stmt->String();
        }
        return out.str();
    }
};

struct Identifier : public Expression {
    token::Token Token;
    std::string Value;

    Identifier(token::Token token) : Token(token), Value(token.Literal) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
    std::string String() const override { return Value; }
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

    std::string String() const override {
        std::stringstream out;
        out << TokenLiteral() + " ";
        out << Name->String();
        out << " = ";
        if (Value != nullptr) {
            out << Value->String();
        }
        out << ";";

        return out.str();
    }

    void statementNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
};

struct ReturnStatement : public Statement {
    token::Token Token;
    Expression* ReturnValue;

    ReturnStatement(token::Token token) : Token(token), ReturnValue(nullptr) {}
    ~ReturnStatement() { delete ReturnValue; }

    std::string String() const override {
        std::stringstream out;
        out << TokenLiteral() + " ";
        if (ReturnValue != nullptr) {
            out << ReturnValue->String();
        }
        out << ";";

        return out.str();
    }

    void statementNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
};

struct ExpressionStatement : public Statement {
    token::Token Token;
    Expression* expression;

    ExpressionStatement(token::Token token) : Token(token), expression(nullptr) {}
    ~ExpressionStatement() { delete expression; }

    std::string String() const override {
        if (expression != nullptr) {
            return expression->String();
        }
        return "";
    }

    void statementNode() override {}
    std::string TokenLiteral() const override { return Token.Literal; }
};

#endif // AST_H
