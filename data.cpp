#include "data.h"

lex::lex(int x, int y, int z, const std::string &n) {
    code = x;
    line = y;
    column = z;
    lexem = n;
}

