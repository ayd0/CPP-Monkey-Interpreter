#include "../../include/eval.h"

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
            {
                ast::PrefixExpression* prexpr = dynamic_cast<ast::PrefixExpression*>(node);
                object::Object* right = Eval(prexpr->Right);
                return evalPrefixExpression(prexpr->Operator, right);
            }
        case ast::NodeType::InfixExpression :
            {
                ast::InfixExpression* infexpr = dynamic_cast<ast::InfixExpression*>(node);
                object::Object* left = Eval(infexpr->Left);
                object::Object* right = Eval(infexpr->Right);
                return evalInfixExpression(infexpr->Operator, left, right);
            }
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

object::Object* evalPrefixExpression(std::string oper, object::Object* right) {
    if (oper == "!") {
        return evalBangOperatorExpression(right);
    } if (oper == "-") {
        return evalMinusPrefixOperatorExpression(right);
    } else {
        return object::NULL_T.get();
    }
}

object::Object* evalBangOperatorExpression(object::Object* right) {
    object::Boolean* boolRight = dynamic_cast<object::Boolean*>(right);
    if (boolRight) {
        if (boolRight->Value) {
            return object::FALSE.get();
        } else {
            return object::TRUE.get();
        }
    }

    object::Null* nullRight = dynamic_cast<object::Null*>(right);
    if (nullRight) {
        return object::TRUE.get();
    }

    return object::FALSE.get();
}

object::Object* evalMinusPrefixOperatorExpression(object::Object* right) {
    if (right->Type() != object::INTEGER_OBJ) {
        return object::NULL_T.get();
    }

    object::Integer* intRight = dynamic_cast<object::Integer*>(right);
    return new object::Integer(-intRight->Value);
}

object::Object* evalInfixExpression(std::string oper, object::Object* right, object::Object* left) {
    if (left->Type() == object::INTEGER_OBJ && right->Type() == object::INTEGER_OBJ) {
        return evalIntegerInfixExpression(oper, left, right);
    } else if (oper == "==") {
        return nativeBoolToBooleanObject(left == right);
    } else if (oper == "!=") {
        return nativeBoolToBooleanObject(left != right);
    }

    return object::NULL_T.get();
}

object::Object* evalIntegerInfixExpression(std::string oper, object::Object* left, object::Object* right) {
    // TODO: determine why right is accumulating values and left
    // reperesnts next node, order is reversed from expected behavior
    object::Integer* intLeft  = dynamic_cast<object::Integer*>(right);
    object::Integer* intRight = dynamic_cast<object::Integer*>(left);
    
    if        (oper == "+") {
        return new object::Integer(intLeft->Value + intRight->Value);
    } else if (oper == "-") {
        return new object::Integer(intLeft->Value - intRight->Value);
    } else if (oper == "*") {
        return new object::Integer(intLeft->Value * intRight->Value);
    } else if (oper == "/") {
        return new object::Integer(intLeft->Value / intRight->Value);
    } else if (oper == "<") {
        return nativeBoolToBooleanObject(intLeft->Value < intRight->Value);
    } else if (oper == ">") {
        return nativeBoolToBooleanObject(intLeft->Value > intRight->Value);
    } else if (oper == "==") {
        return nativeBoolToBooleanObject(intLeft->Value == intRight->Value);
    } else if (oper == "!=") {
        return nativeBoolToBooleanObject(intLeft->Value != intRight->Value);
    }
    
    return object::NULL_T.get();
}

object::Object* nativeBoolToBooleanObject(bool input) {
    if (input) {
        return object::TRUE.get();
    } else {
        return object::FALSE.get();
    }
}
