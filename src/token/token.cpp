#include "../../include/token.h"
#include <map>

std::map<std::string, token::TokenType> keywords = {
    {"fn",     token::FUNCTION},
    {"let",    token::LET},
    {"true",   token::TRUE},
    {"false",  token::FALSE},
    {"if",     token::IF},
    {"else",   token::ELSE},
    {"return", token::RETURN}};

token::TokenType LookupIdent(std::string ident) {
    auto it = keywords.find(ident);
    return it != keywords.end() ? it->second : token::IDENT;
}
