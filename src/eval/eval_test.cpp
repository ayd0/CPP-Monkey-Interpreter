#include "../../include/object.h"
#include "../../include/parser.h"
#include "../../include/eval.h"

#include <string>
#include <variant>

struct LitTest {
    std::string input;
    int64_t     expected;
};

void TestEvalIntegerExpression();
void TestEvalStringExpression();
void TestEvalStringConcatenation();
void TestEvalBooleanExpression();
void TestBangOperator();
void TestIfElseExpressions();
void TestEvalReturnStatements();
void TestErrorHandling(); 
void TestEvalLetStatements();
void TestEvalFunctionObject();
void TestEvalFunctionApplication();
void TestBuiltinFunctions();
void TestArrayLiterals();

object::Object* testEval(std::string input, object::Environment* env);
bool testIntegerObject(object::Object* obj, int64_t expected);
bool testBooleanObject(object::Object* obj, bool expected);
bool testNullObject(object::Object* obj);

/*
int main() {
    TestEvalIntegerExpression();
    TestEvalStringExpression();
    TestEvalStringConcatenation();
    TestEvalBooleanExpression();
    TestBangOperator();
    TestIfElseExpressions();
    TestEvalReturnStatements();
    TestErrorHandling(); 
    TestEvalLetStatements();
    TestEvalFunctionObject();
    TestEvalFunctionApplication();
    TestBuiltinFunctions();
    TestArrayLiterals();

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
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        testIntegerObject(evaluated, test.expected);
        delete env;
    }
}  

void TestEvalStringExpression() {
    std::string input = "\"Hello World!\"";

    object::Environment* env = new object::Environment;
    object::Object* evaluated = testEval(input, env);
    object::String* strObj = dynamic_cast<object::String*>(evaluated);

    if (!strObj) {
        std::cerr << "evaluated is not object::String, got=" << 
            typeid(strObj).name() << std::endl;
        return;
    }

    if (strObj->Value != "Hello World!") {
        std::cerr << "strObj->Value not \"Hello World!\", got=" <<
            strObj->Value << std::endl;
        return;
    }
}

void TestEvalStringConcatenation() {
    std::string input = "\"Hello \" + \"World!\"";

    object::Environment* env = new object::Environment();
    object::Object* evaluated = testEval(input, env);
    object::String* strObj = dynamic_cast<object::String*>(evaluated);

    if (!strObj) {
        std::cerr << "evaluated not object::String, got=" <<
            typeid(strObj).name() << std::endl;
        return;
    }

    if (strObj->Value != "Hello World!") {
        std::cerr << "strObj->Value not \"Hello World!\", got=" <<
            strObj->Value << std::endl;
        return;
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
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        testBooleanObject(evaluated, test.expected);
        delete env;
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
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        testBooleanObject(evaluated, test.expected);
        delete env;
    }
}

void TestIfElseExpressions() {
    LitTest tests[] {
        {"if (true) { 10 }", 10}, 
        {"if (false) { 10 }"},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 }"},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    for (LitTest test : tests) {
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        if (evaluated->Type() == object::INTEGER_OBJ) {
            testIntegerObject(evaluated, test.expected);
        } else {
            testNullObject(evaluated);
        }
        delete env;
    }
}

void TestEvalReturnStatements() {
    LitTest tests[] {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {
            "if (10 > 1) {    "
            "   if (10 > 1) { "
            "       return 10;"
            "   }             "
            "   return 1;     "
            "}                ",
            10
        }
    };

    for (LitTest test : tests) {
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        testIntegerObject(evaluated, test.expected);
        delete env;
    }
}

void TestErrorHandling() {
    struct ErrTest {
        std::string input;
        std::string expectedMsg;
    };

    ErrTest tests[] {
        {
        "5 + true;",
        "type mismatch: BOOLEAN + INTEGER",
        },
        {
        "5 + true; 5;",
        "type mismatch: BOOLEAN + INTEGER",
        },
        {
        "-true",
        "unknown operator: -BOOLEAN",
        },
        {
        "true + false;",
        "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
        "5; true + false; 5",
        "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
        "if (10 > 1) { true + false; }",
        "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
        "if (10 > 1) {       "
        "if (10 > 1) {       "
        "return true + false;"
        "}                   "
        "return 1;           "
        "}                   ",
        "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "foobar",
            "identifier not found: foobar"
        },
        {
            "\"Hello\" - \"World\"",
            "unknown operator: STRING - STRING",
        }
    };

    for (ErrTest test : tests) {
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);
        object::Error* evalErr = dynamic_cast<object::Error*>(evaluated);

        if (!evalErr) {
            std::cerr << "testEval did not return object::Error, got=" << 
                typeid(evaluated).name() << std::endl;
            continue;
        }

        if (evalErr->Message != test.expectedMsg) {
            std::cerr << "evalErr->Message not test.expectedMsg (" << test.expectedMsg 
                << ")" << ", got=" << evalErr->Message << std::endl;
        }
        delete env;
    }
}

void TestEvalLetStatements() {
    LitTest tests[] {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15}
    };

    for (LitTest test : tests) {
        object::Environment* env = new object::Environment();
        testIntegerObject(testEval(test.input, env), test.expected);
        delete env;
    }
}

void TestEvalAssignStatements() {
    LitTest tests[] {
        {"let a = 5; a = 10;", 10},
        {"let a = 5; let b = a; let c = a + b + 5; a = c + b; a;", 20}
    };

    for (LitTest test : tests) {
        object::Environment* env = new object::Environment();
        testIntegerObject(testEval(test.input, env), test.expected);
        delete env;
    }
}

void TestEvalFunctionObject() {
    std::string input = "fn(x) { x + 2; };";

    object::Environment* env = new object::Environment();
    object::Object* evaluated = testEval(input, env);

    object::Function* funcObj = dynamic_cast<object::Function*>(evaluated);
    if (!funcObj) {
        std::cerr << "evaluated not object::Function, got=" <<
            typeid(funcObj).name() << std::endl;
        return;
    }

    if (funcObj->Parameters.size() != 1) {
        std::cerr << "funcObj->Paramaeters size not limit 1, got=" <<
            funcObj->Parameters.size() << std::endl;
        return;
    }

    if (funcObj->Parameters[0]->String() != "x") {
        std::cerr << "funcObj->Paramaeters[0]->String() not 'x', got=" <<
            funcObj->Parameters[0]->String() << std::endl;
        return;
    }

    std::string expectedBody = "(x + 2)";

    if (funcObj->Body->String() != expectedBody) {
        std::cerr << "funcObj->Body->String() not " << expectedBody <<
            " got=" << funcObj->Body->String() << std::endl;
        return;
    }

    delete env;
}

void TestEvalFunctionApplication() {
    LitTest tests[] {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {
            "let add = 0;          "
            "add = fn(x, y) {      "
            "   x + y;             "
            "};                    "
            "add(5 + 5, add(5, 5));"
            , 20
        },
        {
            "let newAdder = fn(x) {   "
            "   fn(y) { x + y }       "
            "};                       "
            "let addTwo = newAdder(2);"
            "addTwo(3);               ",
            5
        },
        {
            "let newAdder = fn(x) {    "
            "   return fn(y) { x + y };"
            "}                         "
            "                          "
            "let addTwo = newAdder(2)  "
            "addTwo(2)                 ",
            4
        },
        {"fn(x) { x; }(5)", 5},
    };

    for (LitTest test : tests) {
        object::Environment* env = new object::Environment();
        testIntegerObject(testEval(test.input, env), test.expected);
    }
}

void TestBuiltinFunctions() {
    struct TestBuiltin {
        std::string input;
        std::variant<int, std::string> expected;
    };

    TestBuiltin tests[] {
        {"len(\"\")", 0},
        {"len(\"four\")", 4},
        {"len(\"hello world\")", 11},
        {"len(1)", "argument to \"len\" not supported, got INTEGER"},
        {"len(\"one\", \"two\")", "wrong number of arguments. got=2, want=1"}
    };

    for (TestBuiltin test : tests) {
        object::Environment* env = new object::Environment();
        object::Object* evaluated = testEval(test.input, env);

        std::visit([evaluated, test](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                testIntegerObject(evaluated, std::get<int>(test.expected));
            } else if constexpr (std::is_same_v<T, std::string>) {
                object::Error* errObj = dynamic_cast<object::Error*>(evaluated);
                if (!errObj) {
                    std::cerr << "evaluated is not object::Error, got=" << 
                        typeid(errObj).name() << std::endl;
                    return;
                }
                std::string expectedErr = std::get<std::string>(test.expected);
                if (errObj->Message != expectedErr) {
                    std::cerr << "errObj->Message not expected error, want=" <<
                        expectedErr << ", got=" << errObj->Message << std::endl;
                }
            }
        }, test.expected);
    }
}

void TestArrayLiterals() {
    std::string input = "[1, 2 * 2, 3 + 3]";

    object::Environment* env = new object::Environment();
    object::Object* evaluated = testEval(input, env);
    object::Array* arr = dynamic_cast<object::Array*>(evaluated);

    if (!arr) {
        std::cerr << "evaluated is not object::Array, got=" <<
            typeid(arr).name() << std::endl;
        return;
    }

    if (arr->Elements.size() != 3) {
        std::cerr << "arr->Elements size not limt 3, got=" <<
            arr->Elements.size() << std::endl;
    }

    testIntegerObject(arr->Elements[0], 1);
    testIntegerObject(arr->Elements[1], 4);
    testIntegerObject(arr->Elements[2], 6);
}

object::Object* testEval(std::string input, object::Environment* env) {
    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();

    return Eval(&program, env);
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

bool testNullObject(object::Object* obj) {
    if (obj != object::NULL_T.get()) {
        std::cerr << "obj is not NULL, got=" << 
            typeid(obj).name() << std::endl;
        return false;
    }
    return true;
}
