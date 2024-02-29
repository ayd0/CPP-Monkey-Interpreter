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
        case ast::NodeType::StringLiteral :
            {
                ast::StringLiteral* strlit = dynamic_cast<ast::StringLiteral*>(node);
                return new object::String(strlit->Value);
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
            {
                ast::FunctionLiteral* funcLit = dynamic_cast<ast::FunctionLiteral*>(node);
                // *********************************************************************************
                // NOTE: cloning function only necessary in cases where Env survives the program,
                //       in the case of the REPL loop, this should not be necessary. 
                // TODO: setup flag to handle disabling deep copy
                // *********************************************************************************
                ast::FunctionLiteral* cp = funcLit->clone();

                std::vector<ast::Identifier*> Parameters = cp->Parameters;
                ast::BlockStatement* Body = cp->Body;

                return new object::Function(Parameters, Body, env);
            }
        case ast::NodeType::AssignExpression :
            {
                // TODO: error handle a + b = 20;
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
            {
                ast::CallExpression* callexpr = dynamic_cast<ast::CallExpression*>(node);
                object::Object* function = Eval(callexpr->Function, env);
                if (isError(function)) {
                    return function;
                }
                std::vector<object::Object*> args = evalExpressions(callexpr->Arguments, env);
                if (args.size() == 1 && isError(args[0])) { 
                    return args[0];
                }
                return applyFunction(function, args);
            }
        case ast::NodeType::ArrayLiteral :
            {
                ast::ArrayLiteral* arrlit = dynamic_cast<ast::ArrayLiteral*>(node);
                std::vector<object::Object*> elements = evalExpressions(arrlit->Elements, env);
                if (elements.size() == 1 && isError(elements[0])) {
                    return elements[0];
                }
                return new object::Array(elements);
            }
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

object::Object* evalProgram(std::vector<ast::Statement*> &stmts, object::Environment* env) {
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
    } else if (left->Type() == object::STRING_OBJ && right->Type() == object::STRING_OBJ) {
        return evalStringInfixExpression(oper, left, right);
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

object::Object* evalStringInfixExpression(std::string oper, object::Object* left, object::Object* right) {
    if (oper != "+") {
        return new object::Error("unknown operator: " + left->Type() + " " + oper + " " + right->Type());
    }

    object::String* leftVal = dynamic_cast<object::String*>(right);
    object::String* rightVal = dynamic_cast<object::String*>(left);
    return new object::String(leftVal->Value + rightVal->Value);
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
        auto it = object::builtins.find(ident->Value);
        if (it != object::builtins.end()) {
            return it->second;
        }
        return new object::Error("identifier not found: " + ident->Value);
    }

    return valOk.first;
}

std::vector<object::Object*> evalExpressions(
        std::vector<ast::Expression*> exprs, 
        object::Environment* env) 
{
    std::vector<object::Object*> result;

    for (ast::Expression* expr : exprs) {
        object::Object* evaluated = Eval(expr, env);
        if (isError(evaluated)) {
            return std::vector<object::Object*>{evaluated};
        }
        result.push_back(evaluated);
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

object::Object* applyFunction(object::Object* fn, std::vector<object::Object*> &args) {
    
    if (fn->Type() == object::FUNCTION_OBJ) {
        object::Function* function = dynamic_cast<object::Function*>(fn);
        object::Environment* extendedEnv = extendFunctionEnv(function, args);
        object::Object* evaluated = Eval(function->Body, extendedEnv);
        return unwrapReturnValue(evaluated);
    } else if (fn->Type() == object::BUILTIN_OBJ) {
        object::Builtin* builtin = dynamic_cast<object::Builtin*>(fn);
        return builtin->BuiltinFunction(args);
    }
    return new object::Error("not a function, got=" + std::string(typeid(fn).name()));

}

object::Environment* extendFunctionEnv(object::Function* fn, std::vector<object::Object*> &args) {
    object::Environment* env = fn->Env->NewEnclosedEnvironment();

    for (unsigned int i = 0; i < fn->Parameters.size(); ++i) {
        env->Set(fn->Parameters[i]->Value, args[i]);
    }

    return env;
}

object::Object* unwrapReturnValue(object::Object* obj) {
    object::ReturnValue* rtrnVal = dynamic_cast<object::ReturnValue*>(obj);
    if (rtrnVal) {
        return rtrnVal->Value;
    }

    return obj;
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
