#include "../../include/token.h"
#include <ios>
#include <map>
#include <iostream>

std::map<std::string, token::TokenType> keywords = {
    {"fn",  token::FUNCTION},
    {"let", token::LET}};

token::TokenType LookupIdent(std::string ident) {
    std::cout << "++++++++++++++++++++REJECTED? `" << std::boolalpha << 
        (keywords.find(ident) == keywords.end()) << std::endl;;
    return keywords.find(ident) != keywords.end() ? keywords[ident] : token::IDENT;
}
