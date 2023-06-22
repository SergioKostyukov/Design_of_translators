#ifndef MAIN_CPP_LEXICAL_ANALYZER_H
#define MAIN_CPP_LEXICAL_ANALYZER_H

#include "data.h"

/*
 * This file contains realization of lexical analyzer for lex showed in main.cpp
 * The part of main grammar that describes lexical units:
     <identifier> --> <letter><string>
     <string> --> <letter><string> |
                 <digit><string> |
                 <empty>
     <unsigned-integer> --> <digit><digits-string>
     <digits-string> --> <digit><digit-string> |
                        <empty>
     <digit> --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
     <letter> --> A | B | C | D | ... | Z
*/

bool file_parsing(const std::string &, Data &);

#endif //MAIN_CPP_LEXICAL_ANALYZER_H
