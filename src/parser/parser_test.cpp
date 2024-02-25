#include "../../include/ast.h"
#include "../../include/lexer.h"
#include "../../include/parser.h"

#include <iostream>
#include <variant>
#include <typeinfo>

struct ParserTest {
    std::string input;
    std::string expectedIdentifier;
    std::variant<int, bool, std::string> expectedValue;
};

bool testLetStatement(Statement *s, std::string name);
void TestLetStatements();

int main() {
    TestLetStatements();
    return 0;
}

void checkParserErrors(Parser &p);

void TestLetStatements() {
    ParserTest tests[] = {
        {"let x = 5;", "x", 5},
        {"let y = true;", "y", true},
        {"let foobar = y;", "foobar", "y"}
    };

    std::string input = 
        "let x = 5;\n"
        "let y = 10;\n"
        "let foobar = 838383;\n";

    Lexer l(input);
    Parser p(l);
    Program program = p.ParseProgram();
    checkParserErrors(p);

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
       Statement *stmt = program.Statements[i]; 
       testLetStatement(stmt, tests[i].expectedIdentifier);
    }
}

void checkParserErrors(Parser &p) {
    std::vector<std::string> errors = p.Errors();
    
    if (errors.size() == 0) {
        return;
    }

    std::cerr << "ERROR::PARSER: Parser has errors: (" << errors.size() << ")" << std::endl;
    for (std::string err : errors) {
        std::cerr << "Parser error: " << err << std::endl;
    }

    return;
}

bool testLetStatement(Statement *s, std::string name) {
    if (s->TokenLiteral() != "let") {
        std::cerr << "s.TokenLiteral not 'let'. got=" << s->TokenLiteral() << std::endl;
        return false;
    }

    LetStatement *letStmt = dynamic_cast<LetStatement*>(s);
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
