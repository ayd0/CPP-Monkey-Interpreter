#include "../../include/repl.h"
#include "../../include/lexer.h"
#include "../../include/parser.h"

void Start(std::istream &in, std::ostream &out) {
    std::string line;
    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return;
        }

        Lexer l(line);
        Parser p(l);
        ast::Program program = p.ParseProgram();

        if (p.Errors().size() != 0) {
            p.checkParserErrors();
            continue;
        }

        out << program.String() << std::endl;

        for (token::Token tok = l.NextToken(); tok.Type != token::EOF_T; tok = l.NextToken()) {
            out << "{Type:" << tok.Type << " Literal:" << tok.Literal << "}" << std::endl; 
        }
    }
}
