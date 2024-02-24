#include "../../include/lexer.h"
#include "../../include/token.h"

#include <vector>
#include <iostream>

void TestNextToken (std::string input) {
    Lexer l = New(input);

    struct Test {
        token::TokenType expectedType;
        std::string expectedLiteral;
    };

    std::vector<Test> tests {
        {token::ASSIGN,    "="},
        {token::PLUS,      "+"},
        {token::LPAREN,    "("},
        {token::RPAREN,    ")"},
        {token::LBRACE,    "{"},
        {token::RBRACE,    "}"},
        {token::COMMA,     ","},
        {token::SEMICOLON, ";"},
        {token::EOF_T,     "EOF"},
    };

    for (auto it = tests.begin(); it != tests.end(); ++it) {
        token::Token tok = NextToken(l);

        if (tok.Type != it->expectedType) {
            std::cerr << "LEXER_TEST:: TokenType wrong: expected=" << it->expectedType <<
                ", got=" << tok.Type << std::endl;
        }

        if (tok.Literal != it->expectedLiteral) {
            std::cerr << "LEXER_TEST:: Literal wrong: expected=" << it->expectedLiteral << 
                ", got=" << tok.Literal << std::endl;
        }
    }
}

int main() {
    TestNextToken("=+(){},;");
    TestNextToken(std::string(
            "let five = 5;\n"
            "let ten = 10;\n"
            "\n"
            "let add = fn(x, y) {\n"
            "    x + y;\n"
            "};\n"
            "\n"
            "let result = add(five, ten);\n"
            ));

    return 0;
}
