#ifndef TRACELOG_H
#define TRACELOG_H
// #define ENABLE_TRACING

#include "token.h"
#include <iostream>

class Tracelog {
public:
    static int nestingLevel;

#ifdef ENABLE_TRACING
    Tracelog(const std::string& functionName, token::Token curToken) 
        : functionName(functionName), tokenName(curToken.Literal) {
        printIndentation("BEGIN " + functionName + ": " + tokenName);
        nestingLevel++;
    }
    ~Tracelog() {
        nestingLevel--;
        printIndentation("END " + functionName + ": " + tokenName);
    }
#else
    Tracelog(const std::string& functionName, token::Token curToken) {}
    ~Tracelog() {}
#endif // ENABLE_TRACING

private:
    std::string functionName;
    std::string tokenName;

    static void printIndentation(const std::string& message) {
        for (int i = 0; i < nestingLevel; ++i) { 
            std::cout << "\t";
        }
        std::cout << message << std::endl;
    }
};

#endif // TRACELOG_H
