#ifndef MAIN_CPP_DATA_H
#define MAIN_CPP_DATA_H
/*
 * This file contains all necessary tables and the definition of simple lexem.
 * There are next tables: 'KeyWords', 'Identifiers', 'Labels', 'Lexemes'.
 *      the lists of 'KeyWords' (and 'Separators') was specified by the main grammar.
 *      the lists of 'Identifiers' and 'labels' are created by the time of reading file.
 *      vector 'Lexemes' contain all info about all file elements (code, line, column, name)
 * */

#include <iostream>
#include <map>
#include <vector>

struct lex {
    lex(int x, int y, int z, const std::string &n);

    int code;
    int line;
    int column;
    std::string lexem;
};

struct Tree {
public:
    Tree() {
        parent = nullptr;
        code = -1;
        depth = 0;
        name = "";
    }

    explicit Tree(Tree *parent) {
        this->parent = parent;
        this->depth = this->parent->depth + 1;
        code = -1;
    }

    Tree(Tree *parent, const std::string &name) {
        this->parent = parent;
        this->parent->child.push_back(this);
        this->code = -1;
        this->name = name;
        this->depth = this->parent->depth + 1;
    }

    Tree(Tree *parent, int code, std::string &name) {
        this->parent = parent;
        this->parent->child.push_back(this);
        this->code = code;
        this->name = name;
        this->depth = this->parent->depth + 1;
    }

    void ShowInfo() const {
        std::string enter = "     ";
        for (int i = 0; i < this->depth; i++) {
            std::cout << enter;
        }
        std::cout << this->name << std::endl;

        for (auto &i: this->child) {
            i->ShowInfo();
        }
    }

    int code;
    int depth;
    std::string name;
    Tree *parent;
    std::vector<Tree *> child;
};

enum class Label_type{
    LABEL_CLEAR,
    LABEL_IN,
    LABEL_OUT,
    LABEL_POINT,
};

struct label {
public:
    int code{};
    int count_uses{};    // використання перед операторами
    bool is_link = false;
    Label_type type = Label_type::LABEL_CLEAR;
};

struct Data {
public:
    const std::map<std::string, int> KeyWords{{"PROGRAM", 301},
                                              {"BEGIN",   302},
                                              {"END",     303},
                                              {"GOTO",    304},
                                              {"LINK",    305},
                                              {"IN",      306},
                                              {"OUT",     307},
                                              {"LABEL",   308}
    };

    std::map<std::string, int> identifiers;
    std::map<std::string, label> labels;
    std::vector<lex> lexemes;
    Tree *root;
    Tree *curr;
    int curr_keyword{};
    std::string program_identifier;
    std::string warning;
};

#endif //MAIN_CPP_DATA_H
