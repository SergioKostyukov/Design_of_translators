#include "lexical_analyzer.h"
#include "syntactic_analyzer.h"
#include "generator.h"
#include "data.h"

/*
 * Variant 11
    <signal-program> --> <program>
    <program> --> PROGRAM <procedure-identifier> ; <block> .
    <block> --> <declarations> BEGIN <statements-list> END
    <declarations> --> <label-declarations>
    <label-declarations> --> LABEL <unsigned-integer> <labels-list>; |
                             <empty>
    <labels-list> --> , <unsigned-integer> <labels-list> |
                             <empty>
    <statements-list> --> <statement> <statements-list> |
                             <empty>
    <statement> --> <unsigned-integer> : <statement> |
                    GOTO <unsigned-integer> ; |
                    LINK <variable-identifier> , <unsigned-integer> ; |
                    IN <unsigned-integer> ; |
                    OUT <unsigned-integer> ;
    <variable-identifier> --> <identifier>
    <procedure-identifier> --> <identifier>
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
/*
',', ':', ';', '.'
44   58   59   46
 */

int main() {
    Data data;
    data.root = new Tree();
    data.curr = new Tree();

    if(!file_parsing("Prog.txt", data)) return 0;
    //if (!file_parsing("FalseTest1.txt", data)) return 0; // No used label
    //if(!file_parsing("FalseTest2.txt", data)) return 0; // Overused label
    //if(!file_parsing("FalseTest3.txt", data)) return 0; // Value don`t link
    //if(!file_parsing("FalseTest4.txt", data)) return 0; // Value in other state

    if (!signal_program(data)) return 0;

    code_generator(data);

    return 0;
}
