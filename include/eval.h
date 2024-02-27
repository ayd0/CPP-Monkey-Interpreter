#ifndef EVAL_H
#define EVAL_H

#include "object.h"
#include "ast.h"

object::Object* Eval(ast::Node* node);
object::Object* evalStatements(std::vector<ast::Statement*> stmts);
object::Object* evalPrefixExpression(std::string oper, object::Object* right);
object::Object* nativeBoolToBooleanObject(bool input);
object::Object* evalBangOperatorExpression(object::Object* right);
object::Object* evalMinusPrefixOperatorExpression(object::Object* right);
object::Object* evalInfixExpression(std::string oper, object::Object* right, object::Object* left);
object::Object* evalIntegerInfixExpression(std::string oper, object::Object* left, object::Object* right);
object::Object* evalIfExpression(ast::IfExpression* ifexpr);
bool            isTruthy(object::Object* obj);

#endif // EVAL_H
