#include "lexical_analyzer.h"

#include <fstream>
#include <iomanip>

using std::cout;
using std::endl;

bool is_digit(const char &symbol) {
    // check is it symbol from 0 to 9
    return (int(symbol) >= 48 && int(symbol) <= 57);
}

bool is_alpha(const char &symbol) {
    // check is it symbol from A to Z
    return (int(symbol) >= 65 && int(symbol) <= 90);
}

bool is_separator(const char &symbol) {
    // check is it one of symbols: ',', ':', ';', '.'
    return (int(symbol) == 44 || int(symbol) == 58 || int(symbol) == 59 || int(symbol) == 46);
}

bool is_white_space(const char &symbol) {
    // check is it one of white space symbols
    return (int(symbol) == 32 || int(symbol) == 8 || int(symbol) == 9 || int(symbol) == 10 || int(symbol) == 13);
}

int Code_identifier(const std::string &buf, const std::map<std::string, int> &KeyW, std::map<std::string, int> &ident,
                    int &curr_code) {
    if (KeyW.contains(buf)) {
        return KeyW.at(buf);
    } else if (ident.contains(buf)) {
        return ident[buf];
    } else {
        ident[buf] = curr_code++;
        return ident[buf];
    }
}

void Show_Table(const std::map<std::string, int> &data, const std::string &type) {
    cout << type << " table: " << endl;
    if (data.empty()) {
        cout << "Empty" << endl;
    }

    for (auto &i: data) {
        cout << std::setw(10) << std::left << i.first << " | " << i.second << endl;
    }
    cout << endl;
}

void Show_Table(const std::map<std::string, label> &data, const std::string &type) {
    cout << type << " table: " << endl;
    if (data.empty()) {
        cout << "Empty" << endl;
    }

    for (auto &i: data) {
        cout << std::setw(10) << std::left << i.first << " | " << i.second.code << endl;
    }
    cout << endl;
}

void Show_data(const std::vector<lex> &data) {
    std::cout << "Lexemes line: " << endl;
    if (data.empty()) {
        cout << "Empty" << endl;
    }

    for (auto &i: data) {
        cout << std::setw(3) << std::left << i.line << " | " << std::setw(3) << i.column << " | " << std::setw(5)
             << i.code << " | " << std::setw(10) << i.lexem << endl;
    }
    cout << endl;
}

bool file_parsing(const std::string &name, Data &data) {
    std::ifstream file(name);

    if (!file.is_open()) {
        cout << "Open error" << endl;
    } else {
        int count_line = 1, count_position = 1;
        int identifier_count = 401, labels_count = 501;
        int buff_pos{}, buff_code{};

        char symbol;
        std::string buf;
        file.get(symbol);
        std::string type{};

        while (!file.eof()) {
            if (is_digit(symbol)) {
                buff_pos = count_position;
                do {
                    buf += symbol;
                    file.get(symbol);
                    count_position++;
                } while (is_digit(symbol));

                if(type != "B"){
                    if (!data.labels.contains(buf)) {
                        data.labels[buf].code = labels_count++;
                        //data.labels[buf].is_declared = true;
                    }else{
                        data.warning += "WARNING: Multiply declared of label " + buf + "\n";
                    }
                    data.lexemes.emplace_back(lex(data.labels[buf].code, count_line, buff_pos, buf));
                }else{
                    if (data.labels.contains(buf)) {
                        data.lexemes.emplace_back(lex(data.labels[buf].code, count_line, buff_pos, buf));
                    }else{
                        data.lexemes.emplace_back(lex(labels_count, count_line, buff_pos, buf));
                    }
                }

                buf.clear();
            } else if (is_alpha(symbol)) {
                buff_pos = count_position;
                do {
                    buf += symbol;
                    file.get(symbol);
                    count_position++;
                } while (is_digit(symbol) || is_alpha(symbol));

                if (buf == "BEGIN") {
                    type = "B";
                }

                buff_code = Code_identifier(buf, data.KeyWords, data.identifiers, identifier_count);
                data.lexemes.emplace_back(lex(buff_code, count_line, buff_pos, buf));
                buf.clear();
            } else if (symbol == '(') {
                file.get(symbol);
                count_position++;
                if (symbol == '*') {
                    while (file.get(symbol)) {
                        count_position++;
                        if (symbol == '*') {
                            file.get(symbol);
                            count_position++;
                            if (symbol == ')') {
                                file.get(symbol);
                                count_position++;
                                break;
                            }

                            if (file.eof()) {
                                cout << "Lexer: Error(line " << count_line << ", column " << count_position
                                     << "): Unlocked comment" << endl;
                                return false;
                            }
                        }

                        if (symbol == 10) {
                            count_line++;
                            count_position = 0;
                        }
                    }
                    if (file.eof()) {
                        cout << "Lexer: Error(line " << count_line << ", column " << count_position
                             << "): Unlocked comment" << endl;
                        return false;
                    }
                } else {
                    cout << "Lexer: Error(line " << count_line << ", column " << count_position
                         << "): Incorrect symbol in comment" << endl;
                    return false;
                }
            } else if (is_separator(symbol)) {
                buf += symbol;
                data.lexemes.emplace_back(lex(int(symbol), count_line, count_position, buf));
                buf.clear();
                count_position++;
                file.get(symbol);
            } else if (is_white_space(symbol)) {
                do {
                    if (symbol == 10) {
                        count_line++;
                        count_position = 0;
                    }
                    count_position++;
                    file.get(symbol);
                    if (file.eof()) {
                        break;
                    }
                } while (is_white_space(symbol));
            } else {
                cout << "Lexer: Error(line " << count_line << ", column " << count_position
                     << "): Incorrect symbol '" << symbol << "'" << endl;
                file.get(symbol);
                count_position++;
                return false;
            }
        }
        type.clear();

        Show_Table(data.KeyWords, "Key words");
        Show_Table(data.identifiers, "Identifiers");
        Show_Table(data.labels, "Labels");
        Show_data(data.lexemes);
    }

    file.close();
    return true;
}
