#include "../../include/ast.h"
#include "../../include/lexer.h"
#include "../../include/parser.h"

#include <iostream>
#include <variant>
#include <typeinfo>

struct ParserTest {
    std::string                              input;
    std::string                              expectedOp;
    std::variant<int64_t, bool, std::string> expectedValue;
};

struct InfixTest {
    std::string                              input;
    std::variant<int64_t, bool, std::string> expectedLValue;
    std::string                              expectedOp;
    std::variant<int64_t, bool, std::string> expectedRValue;
};

struct PrecedenceTest {
    std::string input;
    std::string expected;
};

struct ParamTest {
    std::string input;
    std::vector<std::string> expectedParams;
};

void TestLetStatements();
void TestReturnStatements();
void TestIdentifierExpression();
void TestIntegerLiteralExpression();
void TestBoolExpression();
void TestParsingPrefixExpressions();
void TestParsingInfixExpressions();
void TestOperatorPrecedenceParsing();
void TestIfStatement();
void TestFunctionLiteralParsing();
void TestFunctionParameterParsing();
void TestCallExpressionParsing();
bool testLetStatement(ast::Statement *s, std::string name);
bool testLiteral(ast::Expression *il, std::variant<int64_t, bool, std::string>);
bool testIdentifier(ast::Expression* expr, std::string value);
bool testInfixExpression(
        ast::InfixExpression*                    infexpr, 
        std::variant<int64_t, bool, std::string> left, 
        std::string                              oper, 
        std::variant<int64_t, bool, std::string> right);

int main() {
    TestLetStatements();
    TestReturnStatements();
    TestIdentifierExpression();
    TestIntegerLiteralExpression();
    TestBoolExpression();
    TestParsingPrefixExpressions();
    TestParsingInfixExpressions();
    TestOperatorPrecedenceParsing();
    TestIfStatement();
    TestFunctionLiteralParsing();
    TestFunctionParameterParsing();
    TestCallExpressionParsing();

    return 0;
}

void TestLetStatements() {
    ParserTest tests[] = {
        {"let x = 5;",      "x",      5},
        {"let y = true;",   "y",      true},
        {"let foobar = y;", "foobar", "y"}
    };

    for (ParserTest test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();
        
        if (program.Statements.size() != 1) {
            std::cerr << "program.Statements size not limit 1, got=" <<
                program.Statements.size() << std::endl;
            return;
        }

        ast::Statement* stmt = program.Statements[0];
        if (!testLetStatement(stmt, test.expectedOp)) {
            return;
        }

        ast::LetStatement* letStmt = dynamic_cast<ast::LetStatement*>(stmt);
        if (!testLiteral(letStmt->Value, test.expectedValue)) {
            return;
        }
    }
}

void TestReturnStatements() {
    ParserTest tests[] = {
        {"return 5;",  "NA", 5},
        {"return 10;", "NA", 10},
        {"return x",   "NA", "x"}
    };

    for (ParserTest test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();

        if (program.Statements.size() != 1) {
            std::cerr << "program.Statements size not limit 1 statement, got=" << 
                program.Statements.size() << std::endl;
            return;
        }

        ast::ReturnStatement* rtrnStmt = dynamic_cast<ast::ReturnStatement*>(program.Statements[0]);
        if (!rtrnStmt) {
            std::cerr << "program.Statements[0] not ast::ReturnStatemetn, got=" <<
                typeid(rtrnStmt).name() << std::endl;
        }
        
        if (!testLiteral(rtrnStmt->ReturnValue, test.expectedValue)) {
            return;
        };
    }
}

void TestIdentifierExpression() {
    std::string input = "foobar";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.Statements.size() != 1) {
        std::cerr << "program.Statements does not contain limit 1 statement. got=" << 
            program.Statements.size() << std::endl;
        return;
    }

    ast::Statement *stmt = program.Statements[0];
    ast::ExpressionStatement *exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt);
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ast::ExpressionStatement. got=" <<
            typeid(stmt).name() << std::endl;
        return;
    }

    ast::Identifier *ident = dynamic_cast<ast::Identifier*>(exprStmt->expression);
    if (!ident) {
        std::cerr << "exprStmt->Expression is not ast::Identifier. got=" << 
            typeid(ident).name() << std::endl;
        return;
    }

    if (ident->Value != input) {
        std::cerr << "ident->Value not " << input << ", got=" << ident->Value << std::endl;
    }

    if (ident->TokenLiteral() != input) {
        std::cerr << "ident->TokenLiteral not " << input << ", got=" << 
            ident->TokenLiteral() << std::endl;
    }
}

void TestIntegerLiteralExpression() {
    std::string input = "5";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.Statements.size() != 1) {
        std::cerr << "program.Statements does not contain limit 1 statement. got=" << 
            program.Statements.size() << std::endl;
        return;
    }

    ast::Statement *stmt = program.Statements[0];
    ast::ExpressionStatement *exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt);
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ast::ExpressionStatement. got=" <<
            typeid(stmt).name() << std::endl;
        return;
    }

    ast::IntegerLiteral *ilit = dynamic_cast<ast::IntegerLiteral*>(exprStmt->expression);
    if (!ilit) {
        std::cerr << "exprStmt->Expression is not ast::IntegerLiteral. got=" << 
            typeid(ilit).name() << std::endl;
        return;
    }

    if (ilit->Value != 5) {
        std::cerr << "ident->Value not " << input << ", got=" << ilit->Value << std::endl;
    }

    if (ilit->TokenLiteral() != input) {
        std::cerr << "ident->TokenLiteral not " << input << ", got=" << 
            ilit->TokenLiteral() << std::endl;
    }

}

void TestBoolExpression() {
    std::string input = "true";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.Statements.size() != 1) {
        std::cerr << "program.Statements does not contain limit 1 statement. got=" << 
            program.Statements.size() << std::endl;
        return;
    }

    ast::Statement *stmt = program.Statements[0];
    ast::ExpressionStatement *exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt);
    if (!exprStmt) {
        std::cerr << "program.Statements[0] is not ast::ExpressionStatement. got=" <<
            typeid(stmt).name() << std::endl;
        return;
    }

    ast::Boolean *ilit = dynamic_cast<ast::Boolean*>(exprStmt->expression);
    if (!ilit) {
        std::cerr << "exprStmt->Expression is not ast::Boolean. got=" << 
            typeid(ilit).name() << std::endl;
        return;
    }

    if (!ilit->Value) {
        std::cerr << "ident->Value not " << input << ", got=" << ilit->Value << std::endl;
    }

    if (ilit->TokenLiteral() != input) {
        std::cerr << "ident->TokenLiteral not " << input << ", got=" << 
            ilit->TokenLiteral() << std::endl;
    }

}

void TestParsingPrefixExpressions() {
    ParserTest tests[] = {
        {"!5", "!", 5},
        {"-15", "-", 15},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };
    
    for (auto test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();

        if (program.Statements.size() != 1) {
            std::cerr << "program.Statements does not contain limit 1 statement. got=" << 
                program.Statements.size() << std::endl;
            return;
        }

        ast::Statement *stmt = program.Statements[0];
        ast::ExpressionStatement *exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt);
        if (!exprStmt) {
            std::cerr << "program.Statements[0] is not ast::ExpressionStatement. got=" <<
                typeid(stmt).name() << std::endl;
            return;
        }

        ast::PrefixExpression *pexpr = dynamic_cast<ast::PrefixExpression*>(exprStmt->expression);
        if (!pexpr) {
            std::cerr << "exprStmt->Expression is not ast::PrefixExpression. got=" << 
                typeid(pexpr).name() << std::endl;
            return;
        }

        if (pexpr->Operator != test.expectedOp) {
            std::cerr << "ident->Operator not " << test.expectedOp << ", got=" << pexpr->Operator << std::endl;
        }

        if (!testLiteral(pexpr->Right, test.expectedValue)) {
            return;
        }
    }
}

void TestParsingInfixExpressions() {
    InfixTest tests[] {
        {"5 + 5", 5, "+", 5},
        {"5 - 5", 5, "-", 5},
        {"5 * 5", 5, "*", 5},
        {"5 / 5", 5, "/", 5},
        {"5 > 5", 5, ">", 5},
        {"5 < 5", 5, "<", 5},
        {"5 == 5", 5, "==", 5},
        {"5 != 5", 5, "!=", 5},
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };

    for (auto test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();

        if (program.Statements.size() != 1) {
            std::cerr << "program.Statements does not contain limit 1 statement, got=" << 
                program.Statements.size() << std::endl;
            return;
        }
        
        ast::Statement* stmt = program.Statements[0];
        ast::ExpressionStatement* exprStmt = dynamic_cast<ast::ExpressionStatement*>(stmt);
        if (!exprStmt) {
            std::cerr << "program.Statements[0] is not ast::ExpressionStatement, got=" <<
                typeid(exprStmt).name() << std::endl;
            return;
        }
        
        ast::InfixExpression* iexpr = dynamic_cast<ast::InfixExpression*>(exprStmt->expression);
        if (!iexpr) {
            std::cerr << "exprStmt->expresion is not ast::InfixStatement, got=" <<
                typeid(iexpr).name() << std::endl;
            return;
        }

        if (!testLiteral(iexpr->Left, test.expectedLValue)) {
            return;
        }

        if (iexpr->Operator != test.expectedOp) {
            std::cerr << "iexpr->Operator is not " << test.expectedOp << ", got=" << 
                iexpr->Operator << std::endl;
        }
        
        if (!testLiteral(iexpr->Right, test.expectedRValue)) {
            return;
        }
    }
}

void TestOperatorPrecedenceParsing() {
    PrecedenceTest tests[] {
        {
        "-a * b",
        "((-a) * b)",
        },
        {
        "!-a",
        "(!(-a))",
        },
        {
        "a + b + c",
        "((a + b) + c)",
        },
        {
        "a + b - c",
        "((a + b) - c)",
        },
        {
        "a * b * c",
        "((a * b) * c)",
        },
        {
        "a * b / c",
        "((a * b) / c)",
        },
        {
        "a + b / c",
        "(a + (b / c))",
        },
        {
        "a + b * c + d / e - f",
        "(((a + (b * c)) + (d / e)) - f)",
        },
        {
        "3 + 4; -5 * 5",
        "(3 + 4)((-5) * 5)",
        },
        {
        "5 > 4 == 3 < 4",
        "((5 > 4) == (3 < 4))",
        },
        {
        "5 < 4 != 3 > 4",
        "((5 < 4) != (3 > 4))",
        },
        {
        "3 + 4 * 5 == 3 * 1 + 4 * 5",
        "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
        "true",
        "true",
        },
        {
        "false",
        "false",
        },
        {
        "3 > 5 == false",
        "((3 > 5) == false)",
        },
        {
        "3 < 5 == true",
        "((3 < 5) == true)",
        },
        {
        "1 + (2 + 3) + 4",
        "((1 + (2 + 3)) + 4)",
        },
        {
        "(5 + 5) * 2",
        "((5 + 5) * 2)",
        },
        {
        "2 / (5 + 5)",
        "(2 / (5 + 5))",
        },
        {
        "-(5 + 5)",
        "(-(5 + 5))",
        },
        {
        "!(true == true)",
        "(!(true == true))",
        },
        {
        "a + add(b * c) + d",
        "((a + add((b * c))) + d)",
        },
        {
        "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
        "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
        "add(a + b + c * d / f + g)",
        "add((((a + b) + ((c * d) / f)) + g))",
        },
    };

    for (auto test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();

        std::string actual = program.String();
        if (actual != test.expected) {
            std::cerr << "ERROR::Precedence: expected=" << test.expected << 
                ", got=" << actual << std::endl;
        }
    }
}

void TestIfStatement() {
    std::vector<std::string> inputs {
        "if (x < y) { x }",
        "if (x < y) { x } else { y }"
    };

    for (std::string input : inputs) {
        Lexer l(input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();

        if (program.Statements.size() != 1) {
            std::cerr << "program.Statements does not contain limit 1 statements, got=" << 
                program.Statements.size() << std::endl;
            return;
        }

        ast::ExpressionStatement* exprStmt = dynamic_cast<ast::ExpressionStatement*>(program.Statements[0]);
        if (!exprStmt) {
            std::cerr << "program.Statements[0] not ast::ExpressionStatement, got=" << 
                typeid(exprStmt).name() << std::endl;
            return;
        }

        ast::IfExpression* ifexpr = dynamic_cast<ast::IfExpression*>(exprStmt->expression);
        if (!ifexpr) {
            std::cerr << "exprStmt not ast::IfExpression, got=" << 
                typeid(ifexpr).name() << std::endl;
            return;
        }

        ast::InfixExpression* infexpr = dynamic_cast<ast::InfixExpression*>(ifexpr->Condition);
        if (!infexpr) {
            std::cerr << "infexpr not ast::InfixExpression, got=" <<
                typeid(infexpr).name() << std::endl;
            return;
        }

        if (!testInfixExpression(infexpr, "x", "<", "y")) {
            return;
        }
        
        if (ifexpr->Consequence->Statements.size() != 1) {
            std::cerr << "ifexpr->Consequence->Statements size not limit 1, got="
                << ifexpr->Consequence->Statements.size() << std::endl;
            return;
        }
        
        ast::ExpressionStatement* conExprStmt = 
            dynamic_cast<ast::ExpressionStatement*>(ifexpr->Consequence->Statements[0]);
        if (!conExprStmt) {
            std::cerr << "conExprStmt not ast::ExpressionStatement, got=" << 
                typeid(conExprStmt).name() << std::endl;
            return;
        }
        
        if (!testIdentifier(conExprStmt->expression, "x")) {
            return;
        }

        if (ifexpr->Alternative != nullptr) {
            if (ifexpr->Alternative->Statements[0] != nullptr) {
                if (ifexpr->Alternative->Statements.size() > 1) {
                    std::cerr << "ifexpr->Alternative->Statements size not limit 1, got="
                        << ifexpr->Alternative->Statements.size() << std::endl;
                    return;    
                } else {
                    ast::ExpressionStatement* altExprStmt = 
                        dynamic_cast<ast::ExpressionStatement*>(ifexpr->Alternative->Statements[0]);
                    if (!altExprStmt) {
                        std::cerr << "altExprStmt not ast::ExpressionStatement, got=" << 
                            typeid(altExprStmt).name() << std::endl;
                        return;
                    }

                    if (!testIdentifier(altExprStmt->expression, "y")) {
                        return;
                    }
                }
            } else {
                std::cerr << "ifexpr->Alternative not nullptr for input=" << input << ", typeid=" <<
                    typeid(ifexpr->Alternative).name() << std::endl;
            }
        }
    }
}

void TestFunctionLiteralParsing() {
    std::string input = "fn(x, y) { x + y; }";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.Statements.size() != 1) {
        std::cerr << "program.Statements does not contain limit 1 statements, got="
            << program.Statements.size() << std::endl;
        return;
    }

    ast::ExpressionStatement* exprStmt = dynamic_cast<ast::ExpressionStatement*>(program.Statements[0]);
    if (!exprStmt) {
        std::cerr << "program.Statements[0] not ast::ExpressionStatement, got=" <<
            typeid(exprStmt).name() << std::endl;
        return;
    }

    ast::FunctionLiteral* fnlit = dynamic_cast<ast::FunctionLiteral*>(exprStmt->expression);
    if (!fnlit) {
        std::cerr << "exprStmt not ast::FunctionLiteral, got=" <<
            typeid(fnlit).name() << std::endl;
        return;
    }

    if (fnlit->Parameters.size() != 2) {
        std::cerr << "fnlit->Parameters.size() not limit 2, got=" << 
            fnlit->Parameters.size() << std::endl;
        return;
    }

    testLiteral(fnlit->Parameters[0], "x");
    testLiteral(fnlit->Parameters[1], "y");

    if (fnlit->Body->Statements.size() != 1) {
        std::cerr << "fnlit->Body->Statements.size() not limit 1, got=" << 
            fnlit->Body->Statements.size() << std::endl;
        return;
    }

    ast::ExpressionStatement* bodyExprStmt = dynamic_cast<ast::ExpressionStatement*>(fnlit->Body->Statements[0]);
    if (!bodyExprStmt) {
        std::cerr << "fnlit->Body->Statements[0] not ast::ExpressionStatement, got=" << 
            typeid(bodyExprStmt).name() << std::endl;
        return;
    }

    ast::InfixExpression* bodyStmtInfix = dynamic_cast<ast::InfixExpression*>(bodyExprStmt->expression);
    testInfixExpression(bodyStmtInfix, "x", "+", "y");
}

void TestFunctionParameterParsing() {
    ParamTest tests[] {
        {"fn() {};",        std::vector<std::string>()},
        {"fn(x) {};",       std::vector<std::string>{"x"}},
        {"fn(x, y, z) {};", std::vector<std::string>{"x", "y", "z"}},
    };

    for (ParamTest test : tests) {
        Lexer l(test.input);
        Parser p(l);
        ast::Program program = p.ParseProgram();
        p.checkParserErrors();

        ast::ExpressionStatement* stmt = dynamic_cast<ast::ExpressionStatement*>(program.Statements[0]);
        if (!stmt) {
            std::cerr << "program.Statemetns[0] not ast::ExpressionStatement, got=" << 
                typeid(stmt).name() << std::endl;
            return;
        }

        ast::FunctionLiteral* fnlit = dynamic_cast<ast::FunctionLiteral*>(stmt->expression);
        if (!fnlit) {
            std::cerr << "stmt->expression not ast::FunctionLiteral, got=" << 
                typeid(fnlit).name() << std::endl;
            return;
        }

        if (fnlit->Parameters.size() != test.expectedParams.size()) {
            std::cerr << "length parameters wrong. want=" << test.expectedParams.size() << 
                ", got=" << fnlit->Parameters.size() << std::endl;
            return;
        }

        for (unsigned int i = 0; i < fnlit->Parameters.size(); ++i) {
            testLiteral(fnlit->Parameters[i], test.expectedParams[i]);
        }
    }
}

void TestCallExpressionParsing() {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();
    
    if (program.Statements.size() != 1) {
        std::cerr << "program.Statements size not limit 1, got=" <<
            program.Statements.size() << std::endl;
        return;
    }
    
    ast::ExpressionStatement* exprStmt = dynamic_cast<ast::ExpressionStatement*>(program.Statements[0]);
    if (!exprStmt) {
        std::cerr << "program.Statements[0] not ast::ExprsesionStatement, got=" << 
            typeid(exprStmt).name() << std::endl;
        return;
    }

    ast::CallExpression* callExpr = dynamic_cast<ast::CallExpression*>(exprStmt->expression);
    if (!callExpr) {
        std::cerr << "exprStmt->expression not ast::CallExpression, got=" << 
            typeid(callExpr).name() << std::endl;
        return;
    }
    
    if (!testIdentifier(callExpr->Function, "add")) {
        return;
    }

    if (callExpr->Arguments.size() != 3) {
        std::cerr << "callExpr->Arguments size not limit 3, got=" << 
            callExpr->Arguments.size() << std::endl;
        return;
    }

    testLiteral(callExpr->Arguments[0], 1);
    ast::InfixExpression* firstInfxExpr = dynamic_cast<ast::InfixExpression*>(callExpr->Arguments[1]);
    if (!firstInfxExpr) {
        std::cerr << "callExpr->Arguments[1] not ast::InfixExpression, got=" << 
            typeid(firstInfxExpr).name() << std::endl;
        return;
    }
    testInfixExpression(firstInfxExpr, 2, "*", 3);
    ast::InfixExpression* secndInfxExpr = dynamic_cast<ast::InfixExpression*>(callExpr->Arguments[2]);
    if (!secndInfxExpr) {
        std::cerr << "callExpr->Arguments[1] not ast::InfixExpression, got=" << 
            typeid(secndInfxExpr).name() << std::endl;
        return;
    }
    testInfixExpression(secndInfxExpr, 4, "+", 5);
}

bool testLetStatement(ast::Statement *s, std::string name) {
    if (s->TokenLiteral() != "let") {
        std::cerr << "s.TokenLiteral not 'let'. got=" << s->TokenLiteral() << std::endl;
        return false;
    }

    ast::LetStatement *letStmt = dynamic_cast<ast::LetStatement*>(s);
    if (!letStmt) {
        std::cerr << "s not *LetStatement. got=" << typeid(*s).name() << std::endl;
        return false;
    }

    if (letStmt->Name->Value != name) {
        std::cerr << "letStmt->Name->Value not " << name << 
            ", got=" << letStmt->Name->Value << std::endl;
        return false;
    }

    if (letStmt->Name->TokenLiteral() != name) {
        std::cerr << "letStmt->Name->TokenLiteral() not " << name << 
            ", got=" << letStmt->Name->TokenLiteral() << std::endl;
        return false;
    }

    return true;
}

bool testLiteral(ast::Expression *expr, std::variant<int64_t, bool, std::string> value) {
    if (std::holds_alternative<int64_t>(value)) {
        ast::IntegerLiteral *ilit = dynamic_cast<ast::IntegerLiteral*>(expr);
        if (!ilit) {
            std::cerr << "ilit not ast::IntegerLiteral. got=" << 
                typeid(*ilit).name() << std::endl;
            return false;
        }

        if (ilit->Value != std::get<int64_t>(value)) {
            std::cerr << "ilit->Value not " << std::get<int64_t>(value) << ", got=" 
                << std::get<int64_t>(value) << std::endl;
            return false;
        }

        if (ilit->TokenLiteral() != std::to_string(std::get<int64_t>(value))) {
            std::cerr << "ilit->TokenLiteral not " << std::get<int64_t>(value) << ", got=" 
                << ilit->TokenLiteral() << std::endl;
            return false;
        }
    } else if (std::holds_alternative<bool>(value)) {
        ast::Boolean *boolean = dynamic_cast<ast::Boolean*>(expr);
        if (!boolean) {
            std::cerr << "boolean not ast::Boolean. got=" << 
                typeid(*expr).name() << std::endl;
            return false;
        }

        if (boolean->Value != std::get<bool>(value)) {
            std::cerr << "boolean->Value not" << std::get<bool>(value) << ", got=" 
                << boolean->Value << std::endl;
            return false;
        }

        if (boolean->TokenLiteral() != (std::get<bool>(value) ? "true" : "false")) {
            std::cerr << "boolean->TokenLiteral not " << std::get<bool>(value) << ", got=" 
                << boolean->TokenLiteral() << std::endl;
            return false;
        }
    } else if (std::holds_alternative<std::string>(value)) {
        ast::Identifier *ident = dynamic_cast<ast::Identifier*>(expr);
        if (!ident) {
            std::cerr << "ident not ast::Identifier. got=" << 
                typeid(*expr).name() << std::endl;
            return false;
        }

        if (ident->Value != std::get<std::string>(value)) {
            std::cerr << "ident->Value not" << std::get<std::string>(value) << ", got=" 
                << ident->Value << std::endl;
            return false;
        }

        if (ident->TokenLiteral() != std::get<std::string>(value)) {
            std::cerr << "ident->TokenLiteral not " << std::get<std::string>(value) << ", got=" 
                << ident->TokenLiteral() << std::endl;
            return false;
        }
    }
    return true;
}

bool testIdentifier(ast::Expression* expr, std::string value) {
    ast::Identifier* ident = dynamic_cast<ast::Identifier*>(expr);
    if (!ident) {
        std::cerr << "ident not ast::Identifier, got=" <<
            typeid(ident).name() << std::endl;
        return false;
    }

    if (ident->Value != value) {
        std::cerr << "ident->Value not " << value << 
            ", got=" << value << std::endl;
        return false;
    }

    if (ident->TokenLiteral() != value) {
        std::cerr << "ident->TokenLiteral not " << value << 
            ", got=" << value << std::endl;
        return false;
    }

    return true;
}

bool testInfixExpression(
        ast::InfixExpression*                    infexpr, 
        std::variant<int64_t, bool, std::string> left, 
        std::string                              oper, 
        std::variant<int64_t, bool, std::string> right) 
{
    if (!testLiteral(infexpr->Left, left)) {
        return false;
    }

    if (infexpr->Operator != oper) {
        std::cerr << "infexpr->Operator is not " << oper << 
            ", got=" << infexpr->Operator;
        return false;
    }

    if (!testLiteral(infexpr->Right, right)) {
        return false;
    }

    return true;
}
