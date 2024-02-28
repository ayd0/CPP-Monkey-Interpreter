#ifndef EVAL_H
#define EVAL_H

#include "object.h"
#include "ast.h"

object::Object* Eval(ast::Node* node, object::Environment* env);
object::Object* evalProgram(std::vector<ast::Statement*> stmts, object::Environment* env);
object::Object* evalPrefixExpression(std::string oper, object::Object* right);
object::Object* nativeBoolToBooleanObject(bool input);
object::Object* evalBangOperatorExpression(object::Object* right);
object::Object* evalMinusPrefixOperatorExpression(object::Object* right);
object::Object* evalInfixExpression(std::string oper, object::Object* right, object::Object* left);
object::Object* evalIntegerInfixExpression(std::string oper, object::Object* left, object::Object* right);
object::Object* evalIfExpression(ast::IfExpression* ifexpr, object::Environment* env);
object::Object* evalBlockStatements(ast::BlockStatement* blckStmt, object::Environment* env);
object::Object* evalIdentifier(ast::Identifier* ident, object::Environment* env); 
bool            isTruthy(object::Object* obj);
bool            isError(object::Object* obj);

#endif // EVAL_H
