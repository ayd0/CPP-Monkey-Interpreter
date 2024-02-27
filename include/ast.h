#ifndef AST_H
#define AST_H

#include "token.h"

#include <string>
#include <sstream>
#include <vector>

namespace ast {
    enum class NodeType {
        Program,
        Identifier,
        IntegerLiteral,
        Boolean,
        PrefixExpression,
        InfixExpression,
        BlockStatement,
        IfExpression,
        FunctionLiteral,
        CallExpression,
        LetStatement,
        ReturnStatement,
        ExpressionStatement,
    };

    class Node {
    public:
        virtual ~Node() = default;
        virtual std::string TokenLiteral() const = 0;
        virtual std::string String() const = 0;
        virtual NodeType    GetType() const = 0;
    };

    class Statement : public Node {
    public:
        virtual void statementNode() = 0;
    };

    class Expression : public Node {
    public:
        virtual void expressionNode() = 0;
    };

    struct Program : public Node {
        std::vector<Statement*> Statements;
        bool isEmpty = true;

        Program() {}
        ~Program() {
            for (Statement* stmt : Statements) {
                delete stmt;
            }
            Statements.clear();
        }

        std::string TokenLiteral() const override {
            if (!Statements.empty()) {
                return Statements[0]->TokenLiteral();
            } else {
                return "";
            }
        }

        std::string String() const override {
            std::stringstream out;
            for (Statement* stmt : Statements) {
                out << stmt->String();
            }
            return out.str();
        }

        NodeType GetType() const override { return NodeType::Program; }
    };

    struct Identifier : public Expression {
        token::Token Token;
        std::string Value;

        Identifier(token::Token token) : Token(token), Value(token.Literal) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override { return Value; }
        NodeType GetType() const override { return NodeType::Identifier; }
    };

    struct IntegerLiteral : public Expression {
        token::Token Token;
        int64_t Value;

        IntegerLiteral(token::Token token) : Token(token) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::IntegerLiteral; }
    };

    struct Boolean : public Expression {
        token::Token Token;
        bool Value;

        Boolean(token::Token token, bool value) : Token(token), Value(value) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override       { return Token.Literal; }
        NodeType GetType() const override { return NodeType::Boolean; }
    };

    struct PrefixExpression : public Expression {
        token::Token Token;
        std::string Operator;
        Expression* Right;
        
        PrefixExpression(token::Token token) : Token(token), Operator(token.Literal) {}
        ~PrefixExpression() { delete Right; }

        std::string String() const override {
            std::stringstream out;
            out << "(";
            out << Operator;
            out << Right->String();
            out << ")";

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::PrefixExpression; }
    };

    struct InfixExpression : public Expression {
        token::Token Token;
        Expression* Left;
        std::string Operator;
        Expression* Right;
        
        InfixExpression(token::Token token, Expression* left) 
            : Token(token), Left(left), Operator(token.Literal) {}
        ~InfixExpression() { 
            delete Right; 
            delete Left; 
        }

        std::string String() const override {
            std::stringstream out;
            out << "(";
            out << Left->String();
            out << " " << Operator << " ";
            out << Right->String();
            out << ")";

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::InfixExpression; }
    };

    struct BlockStatement : public Statement {
        token::Token Token;
        std::vector<Statement*> Statements;

        BlockStatement(token::Token token) : Token(token) {}
        ~BlockStatement() {
            for (Statement* stmt : Statements) {
                delete stmt;
            }
            Statements.clear();
        }

        std::string String() const override {
            std::stringstream out;
            for (Statement* stmt : Statements) {
                out << stmt->String();
            }
            return out.str();
        }

        void statementNode() override {};
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::BlockStatement; }
    };

    struct IfExpression : public Expression {
        token::Token Token;
        Expression* Condition;
        BlockStatement* Consequence = nullptr;
        BlockStatement* Alternative = nullptr;

        IfExpression(token::Token token) : Token(token) {}
        ~IfExpression() {
            delete Consequence;
            delete Alternative;
        }

        std::string String() const override {
            std::stringstream out;

            out << "if" << Condition->String() << " " << Consequence->String();

            if (Alternative != nullptr) {
                out << "else " << Alternative->String();
            }

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::IfExpression; }
    };

    struct FunctionLiteral : public Expression {
        token::Token Token;
        std::vector<Identifier*> Parameters;
        BlockStatement* Body;

        FunctionLiteral(token::Token token) : Token(token) {}
        ~FunctionLiteral() {
            delete Body;
            for (Identifier* param : Parameters) {
                delete param;
            }
        }

        std::string String() const override {
            std::stringstream out;
            out << "(";
            for (unsigned int i = 0; i < Parameters.size(); ++i) {
                out << Parameters[i]->String();
                if (i != Parameters.size() - 1) {
                    out << ", ";
                }
            }
            out << ")";
            out << Body->String();
            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::FunctionLiteral; }
    };

    struct CallExpression : public Expression {
        token::Token Token;
        Expression* Function; // Identifier or FunctionLiteral
        std::vector<Expression*> Arguments;

        CallExpression(token::Token token, Expression* func)
            : Token(token), Function(func) {}

        std::string String() const override {
            std::stringstream out;
            out << Function->String() << "(";
            for (unsigned int i = 0; i < Arguments.size(); ++i) {
                out << Arguments[i]->String();
                if (i != Arguments.size() - 1) {
                    out << ", ";
                }
            }
            out << ")";
            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::CallExpression; }
    };

    struct LetStatement : public Statement {
        token::Token Token;
        Identifier* Name;
        Expression* Value;

        LetStatement(token::Token token) : Token(token) {}
        ~LetStatement() { 
            delete Value; 
            delete Name; 
        }

        std::string String() const override {
            std::stringstream out;
            out << TokenLiteral() + " ";
            out << Name->String();
            out << " = ";
            out << Value->String();
            out << ";";

            return out.str();
        }

        void statementNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::LetStatement; }
    };

    struct ReturnStatement : public Statement {
        token::Token Token;
        Expression* ReturnValue;

        ReturnStatement(token::Token token) : Token(token) {}
        ~ReturnStatement() { delete ReturnValue; }

        std::string String() const override {
            std::stringstream out;
            out << TokenLiteral() + " ";
            out << ReturnValue->String();
            out << ";";

            return out.str();
        }

        void statementNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::ReturnStatement; }
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
        NodeType GetType() const override { return NodeType::ExpressionStatement; }
    };
}

#endif // AST_H
