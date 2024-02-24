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
        {token::LET, "let"},
        {token::IDENT, "five"},
        {token::ASSIGN, "="},
        {token::INT, "5"},
        {token::SEMICOLON, ";"},
        {token::LET, "let"},
        {token::IDENT, "ten"},
        {token::ASSIGN, "="},
        {token::INT, "10"},
        {token::SEMICOLON, ";"},
        {token::LET, "let"},
        {token::IDENT, "add"},
        {token::ASSIGN, "="},
        {token::FUNCTION, "fn"},
        {token::LPAREN, "("},
        {token::IDENT, "x"},
        {token::COMMA, ","},
        {token::IDENT, "y"},
        {token::RPAREN, ")"},
        {token::LBRACE, "{"},
        {token::IDENT, "x"},
        {token::PLUS, "+"},
        {token::IDENT, "y"},
        {token::SEMICOLON, ";"},
        {token::RBRACE, "}"},
        {token::SEMICOLON, ";"},
        {token::LET, "let"},
        {token::IDENT, "result"},
        {token::ASSIGN, "="},
        {token::IDENT, "add"},
        {token::LPAREN, "("},
        {token::IDENT, "five"},
        {token::COMMA, ","},
        {token::IDENT, "ten"},
        {token::RPAREN, ")"},
        {token::SEMICOLON, ";"},
        {token::EOF_T, ""},
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
