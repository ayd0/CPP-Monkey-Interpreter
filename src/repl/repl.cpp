#include "../../include/repl.h"
#include "../../include/parser.h"
#include "../../include/eval.h"

void Start(std::istream &in, std::ostream &out) {
    std::string line;
    object::Environment* env = new object::Environment();
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

        object::Object* evaluated = Eval(&program, env);
        if (evaluated != nullptr) {
            out << evaluated->Inspect() << std::endl;
        }

        env->deleteAnonymousValues();
    }
    delete env;
}
