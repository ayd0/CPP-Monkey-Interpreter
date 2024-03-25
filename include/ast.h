#ifndef AST_H
#define AST_H

#include "token.h"

#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace ast {
    enum class NodeType {
        Program,
        Identifier,
        IntegerLiteral,
        StringLiteral,
        Boolean,
        PrefixExpression,
        InfixExpression,
        BlockStatement,
        IfExpression,
        FunctionLiteral,
        AssignExpression,
        CallExpression,
        ArrayLiteral,
        IndexExpression,
        HashLiteral,
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
        virtual Statement* clone() const = 0;
    };

    class Expression : public Node {
    public:
        virtual void expressionNode() = 0;
        virtual Expression* clone() const = 0;
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
        Identifier(const Identifier* other) : Token(other->Token), Value(other->Token.Literal) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override { return Value; }
        NodeType GetType() const override { return NodeType::Identifier; }
        Identifier* clone() const override { return new Identifier(*this); }
    };

    struct IntegerLiteral : public Expression {
        token::Token Token;
        int64_t Value;

        IntegerLiteral(token::Token token) : Token(token) {}
        IntegerLiteral(const IntegerLiteral& other) : Token(other.Token), Value(other.Value) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::IntegerLiteral; }
        IntegerLiteral* clone() const override { return new IntegerLiteral(*this); }
    };

    struct StringLiteral : public Expression {
        token::Token Token;
        std::string Value;

        StringLiteral(token::Token token) : Token(token), Value(token.Literal) {}
        StringLiteral(const StringLiteral& other) : Token(other.Token), Value(other.Value) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::StringLiteral; }
        StringLiteral* clone() const override { return new StringLiteral(*this); }
    };

    struct Boolean : public Expression {
        token::Token Token;
        bool Value;

        Boolean(token::Token token, bool value) : Token(token), Value(value) {}
        Boolean(const Boolean& other) : Token(other.Token), Value(other.Value) {}

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        std::string String() const override       { return Token.Literal; }
        NodeType GetType() const override { return NodeType::Boolean; }
        Boolean* clone() const override { return new Boolean(*this); }
    };

    struct PrefixExpression : public Expression {
        token::Token Token;
        std::string Operator;
        Expression* Right;
        
        PrefixExpression(token::Token token) : Token(token), Operator(token.Literal) {}
        PrefixExpression(const PrefixExpression& other)
            : Token(other.Token), Operator(other.Operator), Right(other.Right->clone()) {}
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
        PrefixExpression* clone() const override { return new PrefixExpression(*this); }
    };

    struct InfixExpression : public Expression {
        token::Token Token;
        Expression* Left;
        std::string Operator;
        Expression* Right;
        
        InfixExpression(token::Token token, Expression* left) 
            : Token(token), Left(left), Operator(token.Literal) {}
        InfixExpression(const InfixExpression& other)
            : Token(other.Token), Left(other.Left->clone()), Operator(other.Operator), Right(other.Right->clone()) {}
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
        InfixExpression* clone() const override { return new InfixExpression(*this); }
    };

    struct BlockStatement : public Statement {
        token::Token Token;
        std::vector<Statement*> Statements;

        BlockStatement(token::Token token) : Token(token) {}
        BlockStatement(const BlockStatement& other) : Token(other.Token) {
            for (const Statement* stmt : other.Statements) {
                Statements.push_back(stmt->clone());
            }
        }
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
        BlockStatement* clone() const override { return new BlockStatement(*this); }
    };

    struct IfExpression : public Expression {
        token::Token Token;
        Expression* Condition;
        BlockStatement* Consequence = nullptr;
        BlockStatement* Alternative = nullptr;

        IfExpression(token::Token token) : Token(token) {}
        IfExpression(const IfExpression& other)
            : Token(other.Token), 
              Condition(other.Condition->clone()),
              Consequence(other.Consequence->clone()), 
              Alternative(other.Alternative->clone()) {}

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
        IfExpression* clone() const override { return new IfExpression(*this); }
    };

    struct FunctionLiteral : public Expression {
        token::Token Token;
        std::vector<Identifier*> Parameters;
        BlockStatement* Body;

        FunctionLiteral(token::Token token) : Token(token) {}
        FunctionLiteral(const FunctionLiteral& other)
            : Token(other.Token), Body(other.Body->clone())
        {
            for (Identifier* ident : other.Parameters) {
                Parameters.push_back(ident->clone());
            }
        }

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
        FunctionLiteral* clone() const override { return new FunctionLiteral(*this); }
    };

    struct AssignExpression : public Expression {
        token::Token Token;
        Identifier* Left;
        Expression* Right;

        AssignExpression(token::Token token, Identifier* left) 
            : Token(token), Left(left) {}
        AssignExpression(const AssignExpression& other)
            : Token(other.Token), Left(other.Left->clone()), Right(other.Right->clone()) {}

        std::string String() const override {
            std::stringstream out;
            out << "(";
            out << Left->String();
            out << " = ";
            out << Right->String();
            out << ")";

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::AssignExpression; }
        AssignExpression* clone() const override { return new AssignExpression(*this); }
    };

    struct CallExpression : public Expression {
        token::Token Token;
        Expression* Function; // Identifier or FunctionLiteral
        std::vector<Expression*> Arguments;

        CallExpression(token::Token token, Expression* func)
            : Token(token), Function(func) {}
        CallExpression(const CallExpression& other)
            : Token(other.Token), Function(other.Function->clone()) 
        {
            for (Expression* arg : other.Arguments) {
                Arguments.push_back(arg->clone());
            }
        }

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
        CallExpression* clone() const override { return new CallExpression(*this); }
    };

    struct ArrayLiteral : public Expression {
        token::Token Token;
        std::vector<Expression*> Elements;

        ArrayLiteral(token::Token token) : Token(token) {}
        ArrayLiteral(const ArrayLiteral& other)
            : Token(other.Token) 
        {
            for (Expression* expr : other.Elements) {
                Elements.push_back(expr->clone());
            }
        }

        ~ArrayLiteral() {
            for (Expression* expr : Elements) {
                delete expr;
            }
        }

        std::string String() const override {
            std::stringstream out;
            out << "[";
            for (unsigned int i = 0; i < Elements.size(); ++i) {
                out << Elements[i]->String();
                if (i < Elements.size() - 1) {
                    out << ", ";
                }
            }
            out << "]";

            return out.str();
        }
    
        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::ArrayLiteral; }
        ArrayLiteral* clone() const override { return new ArrayLiteral(*this); }
    };

    struct IndexExpression : public Expression {
        token::Token Token;
        Expression* Left;
        Expression* Index;

        IndexExpression(token::Token token, Expression* left) : Token(token), Left(left) {}
        IndexExpression(const IndexExpression& other) 
            : Token(other.Token), Left(other.Left->clone()), Index(other.Index->clone()) {}
        ~IndexExpression() {
            delete Left;
            delete Index;
        }

        std::string String() const override {
            std::stringstream out;
            out << "(" << Left->String() << "[" << Index->String() << "])";

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::IndexExpression; }
        IndexExpression* clone() const override { return new IndexExpression(*this); }
    };

    struct HashLiteral : public Expression {
        token::Token Token;
        std::map<Expression*, Expression*> Pairs;

        HashLiteral(token::Token token) : Token(token) {}
        HashLiteral(const HashLiteral& other)
            : Token(other.Token) 
        {
            for (const auto& pair : other.Pairs) {
                Pairs[pair.first->clone()] = pair.second->clone();
            }
        }
        ~HashLiteral() {
            for (const auto& pair : Pairs) {
                delete pair.second;
                delete pair.first;
            }
        }

        std::string String() const override {
            std::stringstream out;

            out << "{";
            for (const auto& pair : Pairs) {
                out << pair.first->String() << ":" << pair.second->String() << ", ";
            }
            if (!Pairs.empty()) {
                out.seekp(-2, std::ios_base::end);
            }
            out << "}";

            return out.str();
        }

        void expressionNode() override {}
        std::string TokenLiteral() const override { return Token.Literal; }
        NodeType GetType() const override { return NodeType::HashLiteral; }
        HashLiteral* clone() const override { return new HashLiteral(*this); }
    };

    struct LetStatement : public Statement {
        token::Token Token;
        Identifier* Name;
        Expression* Value;

        LetStatement(token::Token token) : Token(token) {}
        LetStatement(const LetStatement& other)
            : Token(other.Token), Name(other.Name->clone()), Value(other.Value->clone()) {}
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
        LetStatement* clone () const override { return new LetStatement(*this); }
    };

    struct ReturnStatement : public Statement {
        token::Token Token;
        Expression* ReturnValue;

        ReturnStatement(token::Token token) : Token(token) {}
        ReturnStatement(const ReturnStatement& other) 
            : Token(other.Token), ReturnValue(other.ReturnValue->clone()) {}

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
        ReturnStatement* clone() const override { return new ReturnStatement(*this); }
    };

    struct ExpressionStatement : public Statement {
        token::Token Token;
        Expression* expression;

        ExpressionStatement(token::Token token) : Token(token), expression(nullptr) {}
        ExpressionStatement(const ExpressionStatement& other) 
            : Token(other.Token), expression(other.expression->clone()) {}

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
        ExpressionStatement* clone() const override { return new ExpressionStatement(*this); }
    };
}

#endif // AST_H
