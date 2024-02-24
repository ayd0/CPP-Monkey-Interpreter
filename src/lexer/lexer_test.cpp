#include "../../include/lexer.h"
#include "../../include/token.h"

#include <vector>
#include <iostream>

void TestNextToken (std::string input) {
    Lexer l(input);

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
		{token::BANG, "!"},
		{token::MINUS, "-"},
		{token::SLASH, "/"},
		{token::ASTERISK, "*"},
		{token::INT, "5"},
		{token::SEMICOLON, ";"},
		{token::INT, "5"},
		{token::LT, "<"},
		{token::INT, "10"},
		{token::GT, ">"},
		{token::INT, "5"},
		{token::SEMICOLON, ";"},
		{token::IF, "if"},
		{token::LPAREN, "("},
		{token::INT, "5"},
		{token::LT, "<"},
		{token::INT, "10"},
		{token::RPAREN, ")"},
		{token::LBRACE, "{"},
		{token::RETURN, "return"},
		{token::TRUE, "true"},
		{token::SEMICOLON, ";"},
		{token::RBRACE, "}"},
		{token::ELSE, "else"},
		{token::LBRACE, "{"},
		{token::RETURN, "return"},
		{token::FALSE, "false"},
		{token::SEMICOLON, ";"},
		{token::RBRACE, "}"},
		{token::INT, "10"},
		{token::EQ, "=="},
		{token::INT, "10"},
		{token::SEMICOLON, ";"},
		{token::INT, "10"},
		{token::NOT_EQ, "!="},
		{token::INT, "9"},
		{token::SEMICOLON, ";"},
		{token::EOF_T, ""},
    };

    for (auto it = tests.begin(); it != tests.end(); ++it) {
        token::Token tok = l.NextToken();

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

void TestTokens() {
    TestNextToken(std::string(
        "let five = 5;\n"
        "let ten = 10;\n"
        "\n"
        "let add = fn(x, y) {\n"
        "  x + y;\n"
        "};\n"
        "\n"
        "let result = add(five, ten);\n"
        "!-/*5;\n"
        "5 < 10 > 5;\n"
        "\n"
        "if (5 < 10) {\n"
        "   return true;\n"
        "} else {\n"
        "	return false;\n"
        "}\n"
        "\n"
        "10 == 10;\n"
        "10 != 9;\n"
        ));
}
