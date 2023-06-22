#include "syntactic_analyzer.h"
#include "data.h"
#include <iomanip>

using std::cout;
using std::endl;

/*
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
*/

enum class Syntactic_Errors {
    INCORRECT_SYMBOL = 0, // Incorrect symbol : expected 'symbol'
    INCORRECT_KEYWORD,    // Keyword 'word' expected
    INCORRECT_VALUE,      // Expected <unsigned_integer>
    INCORRECT_LABEL,      // Label has not declared
    INCORRECT_IDENTIFIER, // No such identifier in tables
};

bool program(auto &, Data &);

bool block(auto &, Data &);

bool declarations(auto &, Data &);

bool label_declarations(auto &, Data &);

bool labels_list(auto &, Data &);

bool statement_list(auto &, Data &);

bool statement(auto &, Data &);

bool variable_identifier(auto &, Data &);

bool procedure_identifier(auto &, Data &);

bool identifier(auto &, Data &);

bool unsigned_integer(auto &, Data &);

void Error_message(Syntactic_Errors, auto &, const std::string &);


bool signal_program(Data &data) {
    auto it = data.lexemes.begin();

    data.root->name = "<signal_program>";
    data.root->parent = nullptr;
    if (program(it, data)) {
        //data.root->ShowInfo();
        return true;
    }
    return false;

    //program(it, data);
    //data.root->ShowInfo();
    //return true;
}


bool program(auto &it, Data &data) {
    data.curr = new Tree(data.root, "<program>");
    Tree *elem = nullptr;

    if ((*it).code == 301) { // "PROGRAM"
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        data.curr_keyword = 301;

        if (!procedure_identifier(++it, data)) { return false; } // <procedure_identifier>

        if ((*it).code == 59) { // ";"
            elem = new Tree(data.curr, (*it).code, (*it).lexem);

            if (!block(++it, data)) { return false; } // <block>

            if ((*it).code == 46) { // "."
                elem = new Tree(data.curr, (*it).code, (*it).lexem);
            } else {
                Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ".");
                return false;
            }
        } else {
            Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ";");
            return false;
        }
    } else {
        Error_message(Syntactic_Errors::INCORRECT_KEYWORD, it, "'PROGRAM'");
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool block(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<block>");
    if (!declarations(it, data)) { return false; }
    Tree *elem = nullptr;

    if ((*it).code == 302) { // "BEGIN"
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        it++;
        if (!statement_list(it, data)) { return false; }

        if ((*it).code == 303) { // "END"
            elem = new Tree(data.curr, (*it).code, (*it).lexem);
            it++;
        } else {
            Error_message(Syntactic_Errors::INCORRECT_KEYWORD, it, "'END'");
            return false;
        }
    } else {
        Error_message(Syntactic_Errors::INCORRECT_KEYWORD, it, "'BEGIN'");
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool declarations(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<declarations>");

    if (!label_declarations(it, data)) { return false; }

    data.curr = data.curr->parent;
    return true;
}

bool label_declarations(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<label-declaration>");
    Tree *elem = nullptr;

    if ((*it).code != 302 && (*it).code != 308) {
        Error_message(Syntactic_Errors::INCORRECT_KEYWORD, it, "'LABEL'");
        return false;
    }

    bool is_any_label = false;
    while ((*it).code == 308) { // "LABEL"
        is_any_label = true;
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        it++;

        if (!unsigned_integer(it, data)) {
            Error_message(Syntactic_Errors::INCORRECT_VALUE, it, "");
            return false;
        }

        if (!labels_list(it, data)) { return false; }

        if ((*it).code == 59) { // ";"
            elem = new Tree(data.curr, (*it).code, (*it).lexem);
        } else {
            Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ";");
            return false;
        }
        it++;
    }

    if (!is_any_label) {
        elem = new Tree(data.curr, "<empty>");
    }

    data.curr = data.curr->parent;
    return true;
}

bool labels_list(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<labels-list>");
    Tree *elem;

    while ((*it).code == 44) { // ","
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        it++;
        if (!unsigned_integer(it, data)) {
            Error_message(Syntactic_Errors::INCORRECT_VALUE, it, "");
            return false;
        }
    }

    elem = new Tree(data.curr, "<empty>");

    data.curr = data.curr->parent;
    return true;
}

bool statement_list(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<statement-list>");

    while ((*it).code != 303) { // END
        if (!statement(it, data)) {
            return false;
        }
    }

    Tree *elem = new Tree(data.curr, "<empty>");

    data.curr = data.curr->parent;
    return true;
}

bool statement(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<statement>");
    Tree *elem = nullptr;
    data.curr_keyword = 0;

    if (unsigned_integer(it, data)) {
        if ((*it).code == 58) { // ":"
            elem = new Tree(data.curr, (*it).code, (*it).lexem);
            it++;
        } else {
            Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ":");
            return false;
        }
    } else if (!((*it).code == 304 || (*it).code == 306 || (*it).code == 307 || (*it).code == 305)) {
        Error_message(Syntactic_Errors::INCORRECT_LABEL, it, "");
        return false;
    }

    if ((*it).code == 304 || (*it).code == 306 || (*it).code == 307) { // "GOTO", "IN", "OUT"
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        data.curr_keyword = (*it).code;
        it++;
        if (!unsigned_integer(it, data)) {
            Error_message(Syntactic_Errors::INCORRECT_LABEL, it, "");
            return false;
        }
        if ((*it).code == 59) { // ";"
            elem = new Tree(data.curr, (*it).code, (*it).lexem);
            it++;
        } else {
            Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ";");
            return false;
        }
    } else if ((*it).code == 305) { // "LINK"
        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        data.curr_keyword = 305;
        it++;

        if (!variable_identifier(it, data)) {
            return false;
        }

        if ((*it).code == 44) { // ","
            elem = new Tree(data.curr, (*it).code, (*it).lexem);
            it++;
            if (!unsigned_integer(it, data)) {
                Error_message(Syntactic_Errors::INCORRECT_LABEL, it, "");
                return false;
            }
            if ((*it).code == 59) { // ";"
                elem = new Tree(data.curr, (*it).code, (*it).lexem);
                it++;
            } else {
                Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ";");
                return false;
            }
        } else {
            Error_message(Syntactic_Errors::INCORRECT_SYMBOL, it, ",");
            return false;
        }
    } else {
        Error_message(Syntactic_Errors::INCORRECT_KEYWORD, it, "one of: 'LINK', 'GOTO', 'IN', 'OUT', 'END'");
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool variable_identifier(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<variable-identifier>");

    if (!identifier(it, data)) {
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool procedure_identifier(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<procedure-identifier>");

    if (!identifier(it, data)) {
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool identifier(auto &it, Data &data) {
    data.curr = new Tree(data.curr, "<identifier>");
    Tree *elem = nullptr;

    if (data.identifiers.contains((*it).lexem)) {
        elem = new Tree(data.curr, (*it).code, (*it).lexem);

        if (data.curr_keyword == 301) {
            data.program_identifier = (*it).lexem;
        }

        it++;
    } else {
        Error_message(Syntactic_Errors::INCORRECT_IDENTIFIER, it, "");
        return false;
    }

    data.curr = data.curr->parent;
    return true;
}

bool unsigned_integer(auto &it, Data &data) {
    if ((*it).code == 304 || (*it).code == 305 || (*it).code == 306 || (*it).code == 307) {
        return false;
    }
    data.curr = new Tree(data.curr, "<unsigned-integer>");
    Tree *elem = nullptr;

    if (data.labels.contains((*it).lexem)) {
        if (data.curr_keyword == 0) {
            data.labels[(*it).lexem].type = Label_type::LABEL_POINT;
            data.labels[(*it).lexem].count_uses++;
        }

        elem = new Tree(data.curr, (*it).code, (*it).lexem);
        it++;
    } else {
        data.curr = data.curr->parent;
        return false;
    }

    data.curr = data.curr->parent;

    return true;
}

void Error_message(Syntactic_Errors error, auto &it, const std::string &description) {
    cout << "Parser: Error (line " << (*it).line << ", column " << (*it).column << "): ";
    if (error == Syntactic_Errors::INCORRECT_SYMBOL) {
        cout << "Incorrect symbol '" << (*it).lexem << "', " << "expected '" << description << "'" << endl;
    } else if (error == Syntactic_Errors::INCORRECT_KEYWORD) {
        cout << "Incorrect KeyWord '" << (*it).lexem << "', " << "expected " << description << endl;
    } else if (error == Syntactic_Errors::INCORRECT_IDENTIFIER) {
        cout << "Incorrect identifier '" << (*it).lexem << "'";
    } else if (error == Syntactic_Errors::INCORRECT_VALUE) {
        cout << "Incorrect value '" << (*it).lexem << "'" << endl;
    } else if (error == Syntactic_Errors::INCORRECT_LABEL) {
        cout << "Incorrect label '" << (*it).lexem << "': Label has not declared" << endl;
    } else {
        cout << "Other Error" << endl;
    }
}