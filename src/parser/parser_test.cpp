#include "../../include/ast.h"
#include "../../include/lexer.h"
#include "../../include/parser.h"

#include <iostream>
#include <variant>
#include <typeinfo>

struct ParserTest {
    std::string input;
    std::string expectedOp;
    std::variant<int64_t, bool, std::string> expectedValue;
};

void TestLetStatements();
void TestReturnStatements();
void TestIdentifierExpression();
void TestIntegerLiteralExpression();
void TestParsingPrefixExpressions();
bool testLetStatement(ast::Statement *s, std::string name);
bool testIntegerLiteral(ast::Expression *il, int64_t value);

int main() {
    TestLetStatements();
    TestReturnStatements();
    TestIdentifierExpression();
    TestIntegerLiteralExpression();
    TestParsingPrefixExpressions();

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

void TestParsingPrefixExpressions() {
    ParserTest tests[] = {
        {"!5", "!", 5},
        {"-15", "-", 15},
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

        if (!testIntegerLiteral(pexpr->Right, std::get<int64_t>(test.expectedValue))) {
            return;
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

bool testIntegerLiteral(ast::Expression *il, int64_t value) {
    ast::IntegerLiteral *ilit = dynamic_cast<ast::IntegerLiteral*>(il);
    if (!ilit) {
        std::cerr << "il not ast::IntegerLiteral. got=" << 
            typeid(*ilit).name() << std::endl;
        return false;
    }

    if (ilit->Value != value) {
        std::cerr << "ilit->Value not " << value << ", got=" 
            << value << std::endl;
        return false;
    }

    if (ilit->TokenLiteral() != std::to_string(value)) {
        std::cerr << "ilit->TokenLiteral not " << value << ", got=" 
            << ilit->TokenLiteral() << std::endl;
        return false;
    }

    return true;
}
