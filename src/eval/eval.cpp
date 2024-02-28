#include "../../include/eval.h"
#include <iostream>

object::Object* Eval(ast::Node* node, object::Environment* env) {
    switch(node->GetType()) {
        case ast::NodeType::Program :
            {
                return evalProgram(dynamic_cast<ast::Program*>(node)->Statements, env);
            }
        case ast::NodeType::Identifier :
            {
                ast::Identifier* ident = dynamic_cast<ast::Identifier*>(node);
                return evalIdentifier(ident, env); 
            }
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
                object::Object* right = Eval(prexpr->Right, env);
                if (isError(right)) return right;
                return evalPrefixExpression(prexpr->Operator, right);
            }
        case ast::NodeType::InfixExpression :
            {
                ast::InfixExpression* infexpr = dynamic_cast<ast::InfixExpression*>(node);
                object::Object* left = Eval(infexpr->Left, env);
                if (isError(left)) return left;
                object::Object* right = Eval(infexpr->Right, env);
                if (isError(right)) return right;
                return evalInfixExpression(infexpr->Operator, left, right);
            }
        case ast::NodeType::BlockStatement :
            {
                ast::BlockStatement* blockStmt = dynamic_cast<ast::BlockStatement*>(node);
                return evalBlockStatements(blockStmt, env);
            }
        case ast::NodeType::IfExpression :
            {
                ast::IfExpression* ifexpr = dynamic_cast<ast::IfExpression*>(node);
                return evalIfExpression(ifexpr, env);
            }
        case ast::NodeType::FunctionLiteral :
            return nullptr;
        case ast::NodeType::AssignExpression :
            {
                ast::AssignExpression* asexpr = dynamic_cast<ast::AssignExpression*>(node);
                std::pair<object::Object*, bool> valOk = env->Get(asexpr->Left->Value);
                if (!valOk.second) {
                    return new object::Error("identifier not found: " + asexpr->Left->Value);
                }
                object::Object* right = Eval(asexpr->Right, env);
                env->Set(asexpr->Left->Value, right);
                return nullptr;
            }
        case ast::NodeType::CallExpression :
            return nullptr;
        case ast::NodeType::LetStatement :
            {
                ast::LetStatement* letStmt = dynamic_cast<ast::LetStatement*>(node);
                object::Object* val = Eval(letStmt->Value, env);

                if (isError(val)) {
                    return val;
                }
                val->isAnonymous = false;
                env->Set(letStmt->Name->Value, val);
                return nullptr;
            }
        case ast::NodeType::ReturnStatement :
            {
                ast::ReturnStatement* rtrnStmt = dynamic_cast<ast::ReturnStatement*>(node);
                object::Object* val = Eval(rtrnStmt->ReturnValue, env); 
                if (isError(val)) return val;
                return new object::ReturnValue(val);
            }
        case ast::NodeType::ExpressionStatement :
            {
                return Eval(dynamic_cast<ast::ExpressionStatement*>(node)->expression, env);
            }
        default :
            return nullptr;
    }
}

object::Object* evalProgram(std::vector<ast::Statement*> stmts, object::Environment* env) {
    object::Object* result = nullptr;

    for (ast::Statement* stmt : stmts) {
        result = Eval(stmt, env);

        object::ReturnValue* rtrnVal = dynamic_cast<object::ReturnValue*>(result);
        if (rtrnVal) {
            return rtrnVal->Value;
        } else {
            object::Error* errObj = dynamic_cast<object::Error*>(result);
            if (errObj) {
                return errObj;
            }
        }
    }

    return result;
}

object::Object* evalPrefixExpression(std::string oper, object::Object* right) {
    if (oper == "!") {
        return evalBangOperatorExpression(right);
    } if (oper == "-") {
        return evalMinusPrefixOperatorExpression(right);
    } else {
        return new object::Error("unknown operator: " + oper + " " + right->Type());
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
        return new object::Error("unknown operator: -" + right->Type());
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
    } else if (left->Type() != right->Type()) {
        return new object::Error("type mismatch: " + left->Type()
                + " " + oper + " " + right->Type());
    } 

    return new object::Error("unknown operator: " + left->Type()
            + " " + oper + " " + right->Type());
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
    
    return new object::Error("unkown operator: " + left->Type() + 
            " " + oper + " " + right->Type());
}

object::Object* evalIfExpression(ast::IfExpression* ifexpr, object::Environment* env) {
    if (!ifexpr) {
        std::cerr << "ifexpr not ast::IfExpression, got=" << 
            typeid(ifexpr).name() << std::endl;
        return nullptr;
    }

    object::Object* condition = Eval(ifexpr->Condition, env);
    if (isError(condition)) return condition;

    if (isTruthy(condition)) {
        return Eval(ifexpr->Consequence, env);
    } else if (ifexpr->Alternative != nullptr) {
        return Eval(ifexpr->Alternative, env);
    } else {
        return object::NULL_T.get();
    }
}

object::Object* evalBlockStatements(ast::BlockStatement* blckStmt, object::Environment* env) {
    object::Object* result = nullptr;

    for (ast::Statement* stmt : blckStmt->Statements) {
        result = Eval(stmt, env);

        if (result != nullptr) {
            std::string rt = result->Type();
            if (rt == object::RETURN_VALUE_OBJ || rt == object::ERROR_OBJ) {
                return result;
            }
        }
    }

    return result;
}

object::Object* evalIdentifier(ast::Identifier* ident, object::Environment* env) {
    std::pair<object::Object*, bool> valOk = env->Get(ident->Value);
    if (!valOk.second) {
        return new object::Error("identifier not found: " + ident->Value);
    }

    return valOk.first;
}

object::Object* nativeBoolToBooleanObject(bool input) {
    if (input) {
        return object::TRUE.get();
    } else {
        return object::FALSE.get();
    }
}

bool isTruthy(object::Object* obj) {
    if (obj == object::NULL_T.get()) {
        return false;
    } else if (obj == object::TRUE.get()) {
        return true;
    } else if (obj == object::FALSE.get()) {
        return false;
    } else {
        return true;
    }
}

bool isError(object::Object* obj) {
    if (obj != nullptr) {
        return obj->Type() == object::ERROR_OBJ;
    }
    return false;
}
