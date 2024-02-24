#include "../../include/repl.h"
#include "../../include/lexer.h"

void Start(std::istream &in, std::ostream &out) {
    std::string line;
    while (true) {
        out << PROMPT;
        if (!std::getline(in, line)) {
            return;
        }

        Lexer l(line);

        for (token::Token tok = l.NextToken(); tok.Type != token::EOF_T; tok = l.NextToken()) {
            out << "{Type:" << tok.Type << " Literal:" << tok.Literal << "}" << std::endl; 
        }
    }
}
