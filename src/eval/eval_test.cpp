#include "../../include/object.h"
#include "../../include/parser.h"
#include "../../include/eval.h"

#include <string>

struct LitTest {
    std::string input;
    int64_t     expected;
};

void TestEvalIntegerExpression();
void TestEvalBooleanExpression();

object::Object* testEval(std::string input);
bool testIntegerObject(object::Object* obj, int64_t expected);
bool testBooleanObject(object::Object* obj, bool expected);

/*
int main() {
    TestEvalIntegerExpression();
    TestEvalBooleanExpression();

    return 0;
}
*/

void TestEvalIntegerExpression() {
    LitTest tests[] {
        {"5", 5},
        {"10", 10}
    };

    for (LitTest test : tests) {
        object::Object* evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}  

void TestEvalBooleanExpression() {
    LitTest tests[] {
        {"true", true},
        {"false", false}
    };

    for (LitTest test : tests) {
        object::Object* evaluated = testEval(test.input);
        testBooleanObject(evaluated, test.expected);
    }
}

object::Object* testEval(std::string input) {
    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();

    return Eval(&program);
}

bool testIntegerObject(object::Object* obj, int64_t expected) {
    object::Integer* intObj = dynamic_cast<object::Integer*>(obj); 
    if (!intObj) {
        std::cerr << "obj not object::Integer, got=" << 
            typeid(intObj).name() << std::endl;
        return false;
    }

    if (intObj->Value != expected) {
        std::cerr << "intObj->Value incorrect. got=" 
            << intObj->Value << ", want=" << expected << std::endl;
        return false;
    }

    return true;
}

bool testBooleanObject(object::Object* obj, bool expected) {
    object::Boolean* boolObj = dynamic_cast<object::Boolean*>(obj); 
    if (!boolObj) {
        std::cerr << "obj not object::Boolean, got=" << 
            typeid(boolObj).name() << std::endl;
        return false;
    }

    if (boolObj->Value != expected) {
        std::cerr << "boolObj->Value incorrect. got=" 
            << boolObj->Value << ", want=" << expected << std::endl;
        return false;
    }

    return true;
}
