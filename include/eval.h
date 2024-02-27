#ifndef EVAL_H
#define EVAL_H

#include "object.h"
#include "ast.h"

object::Object* Eval(ast::Node* node);
object::Object* evalStatements(std::vector<ast::Statement*> stmts);
object::Object* nativeBoolToBooleanObject(bool input);

#endif // EVAL_H
