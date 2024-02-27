#include "../../include/eval.h"
#include <iostream>

object::Object* Eval(ast::Node* node) {
    switch(node->GetType()) {
        case ast::NodeType::Program :
            {
                return evalStatements(dynamic_cast<ast::Program*>(node)->Statements);
            }
        case ast::NodeType::Identifier :
            return nullptr;
        case ast::NodeType::IntegerLiteral : 
            {
                ast::IntegerLiteral* ilit = dynamic_cast<ast::IntegerLiteral*>(node);
                return new object::Integer(ilit->Value);
            }
        case ast::NodeType::Boolean :
            {
                ast::Boolean* boolit = dynamic_cast<ast::Boolean*>(node);
                return nativeBoolToBooleanObject(boolit->Value);
            }
        case ast::NodeType::PrefixExpression :
            return nullptr;
        case ast::NodeType::InfixExpression :
            return nullptr;
        case ast::NodeType::BlockStatement :
            return nullptr;
        case ast::NodeType::IfExpression :
            return nullptr;
        case ast::NodeType::FunctionLiteral :
            return nullptr;
        case ast::NodeType::CallExpression :
            return nullptr;
        case ast::NodeType::LetStatement :
            return nullptr;
        case ast::NodeType::ReturnStatement :
            return nullptr;
        case ast::NodeType::ExpressionStatement :
            {
                return Eval(dynamic_cast<ast::ExpressionStatement*>(node)->expression);
            }
        default :
            return nullptr;
    }
}

object::Object* evalStatements(std::vector<ast::Statement*> stmts) {
    object::Object* result;

    for (ast::Statement* stmt : stmts) {
        result = Eval(stmt);
    }

    return result;
}

object::Object* nativeBoolToBooleanObject(bool input) {
    if (input) {
        return object::TRUE.get();
    } else {
        return object::FALSE.get();
    }
}
