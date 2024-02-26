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

void TestLetStatements();
void TestReturnStatements();
void TestIdentifierExpression();
void TestIntegerLiteralExpression();
void TestBoolExpression();
void TestParsingPrefixExpressions();
void TestParsingInfixExpressions();
void TestOperatorPrecedenceParsing();
bool testLetStatement(ast::Statement *s, std::string name);
bool testLiteral(ast::Expression *il, std::variant<int64_t, bool, std::string>);
bool testIdentifier(ast::Expression* expr, std::string value);

int main() {
    TestLetStatements();
    TestReturnStatements();
    TestIdentifierExpression();
    TestIntegerLiteralExpression();
    TestBoolExpression();
    TestParsingPrefixExpressions();
    TestParsingInfixExpressions();
    TestOperatorPrecedenceParsing();

    return 0;
}

void TestLetStatements() {
    ParserTest tests[] = {
        {"let x = 5;",      "x",      5},
        {"let y = true;",   "y",      true},
        {"let foobar = y;", "foobar", "y"}
    };

    std::string input = 
        "let x = 5;\n"
        "let y = 10;\n"
        "let foobar = 838383;\n";

    Lexer l(input);
    Parser p(l);
    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.isEmpty) {
        std::cerr << "ParseProgram() is empty" << std::endl;
        return;
    }

    if (program.Statements.size() != 3) {
        std::cerr << "program.Statements does not contain 3 statements. got=" << 
            program.Statements.size() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        ast::Statement *stmt = program.Statements[i]; 
       testLetStatement(stmt, tests[i].expectedOp);
    }
}

void TestReturnStatements() {
    std::string input = 
        "return 5;\n"
        "return 10;\n"
        "return 993322;\n";

    ParserTest tests[] = {
        {"return 5;",     "NA", 5},
        {"return 10;",    "NA", 10},
        {"return 993322", "NA", 993322}
    };

    Lexer l(input);
    Parser p(l);

    ast::Program program = p.ParseProgram();
    p.checkParserErrors();

    if (program.Statements.size() != 3) {
        std::cerr << "program.Statements does not contain 3 statements. got=" << 
            program.Statements.size() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        ast::Statement *stmt = program.Statements[i]; 
        ast::ReturnStatement *returnStmt = dynamic_cast<ast::ReturnStatement*>(stmt);
        if (!returnStmt) {
            std::cerr << "s not *LetStatement. got=" << typeid(stmt).name() << std::endl;
            return;
        }
        if (stmt->TokenLiteral() != "return") {
            std::cerr << "stmt.TokenLiteral not 'return', got=" << stmt->TokenLiteral() << std::endl;
        }
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
        }
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

