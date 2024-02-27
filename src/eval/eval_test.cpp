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
void TestBangOperator();

object::Object* testEval(std::string input);
bool testIntegerObject(object::Object* obj, int64_t expected);
bool testBooleanObject(object::Object* obj, bool expected);

/*
int main() {
    TestEvalIntegerExpression();
    TestEvalBooleanExpression();
    TestBangOperator();

    return 0;
}
*/

void TestEvalIntegerExpression() {
    LitTest tests[] {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}
    };

    for (LitTest test : tests) {
        object::Object* evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}  

void TestEvalBooleanExpression() {
    LitTest tests[] {
        {"true", true},
        {"false", false},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true}
    };

    for (LitTest test : tests) {
        object::Object* evaluated = testEval(test.input);
        testBooleanObject(evaluated, test.expected);
    }
}

void TestBangOperator() {
    LitTest tests[] {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true}
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
