#include "generator.h"

enum class Generator_Errors {
    ERROR_NO_USED_LABEL,            // мітка не використовувалась перед операторами
    ERROR_OVER_USED_LABEL,          // мітка використовувалась перед операторами більше одного разу
    ERROR_VALUE_DONT_LINK,          // порт не був LINK-ований
    ERROR_VALUE_IN_OTHER_STATE,     // порт вже використовується для IN/OUT
};

void Error_message(Generator_Errors, const std::string &);

bool iterations(Tree &curr, std::string &result, Data &data) {
    if (curr.code == 301) { // PROGRAM
        result += ";" + data.program_identifier + "\n\ndata SEGMENT\n";
        data.curr_keyword = 301;
    } else if (curr.code == 302) { // BEGIN
        result += "\ndata ENDS \n\ncode SEGMENT\nbegin:\n";
        data.curr_keyword = 302;
    } else if (curr.code == 303) { // END
        result += "\n\tret 0\ncode ENDS\n\tend begin\n";
    } else if (curr.code == 304) { // GOTO
        data.curr_keyword = 304;
    } else if (curr.code == 305) { // LINK
        data.curr_keyword = 305;
    } else if (curr.code == 306) { // IN
        data.curr_keyword = 306;
    } else if (curr.code == 307) { // OUT
        data.curr_keyword = 307;
    }

    if (curr.parent && curr.parent->name == "<unsigned-integer>") {
        if (data.curr_keyword == 304) { // GOTO
            if (data.labels[curr.name].count_uses == 1) {
                result += "\tJMP " + curr.name + "\n";
            } else if (data.labels[curr.name].count_uses == 0) {
                Error_message(Generator_Errors::ERROR_NO_USED_LABEL, curr.name);
                return false;
            } else {
                Error_message(Generator_Errors::ERROR_OVER_USED_LABEL, curr.name);
                return false;
            }
            data.curr_keyword = 0;
        } else if (data.curr_keyword == 305) { // LINK
            if (data.labels[curr.name].type == Label_type::LABEL_POINT) {
                Error_message(Generator_Errors::ERROR_VALUE_IN_OTHER_STATE, curr.name + "(it is Point label)");
                return false;
            } else {
                data.labels[curr.name].is_link = true;
            }
            data.curr_keyword = 0;
        } else if (data.curr_keyword == 306) { // IN
            if (!data.labels[curr.name].is_link) {
                Error_message(Generator_Errors::ERROR_VALUE_DONT_LINK, curr.name);
                return false;
            } else if (data.labels[curr.name].type == Label_type::LABEL_OUT) {
                Error_message(Generator_Errors::ERROR_VALUE_IN_OTHER_STATE, curr.name + "(used in OUT state)");
                return false;
            } else {
                data.labels[curr.name].type = Label_type::LABEL_IN;
            }
            data.curr_keyword = 0;
        } else if (data.curr_keyword == 307) { // OUT
            if (!data.labels[curr.name].is_link) {
                Error_message(Generator_Errors::ERROR_VALUE_DONT_LINK, curr.name);
                return false;
            } else if (data.labels[curr.name].type == Label_type::LABEL_IN) {
                Error_message(Generator_Errors::ERROR_VALUE_IN_OTHER_STATE, curr.name + "(used in IN state)");
                return false;
            } else {
                data.labels[curr.name].type = Label_type::LABEL_OUT;
            }
            data.curr_keyword = 0;
        } else if (data.labels[curr.name].type == Label_type::LABEL_POINT && data.curr_keyword != 301) {
            result += "\t" + curr.name + ":\n";
        } else if (data.curr_keyword == 302) {
            data.curr_keyword = 0;
        }
    }

    for (auto &i: curr.child) {
        if (!iterations(*i, result, data)) return false;
    }

    return true;
};

void code_generator(Data &data) {
    std::string result;
    data.curr_keyword = 0;

    if (iterations(*data.root, result, data)) {
        std::cout << result << std::endl;
    }

    if (!data.warning.empty()) {
        std::cout << "\n" << data.warning << std::endl;
    }
}

void Error_message(Generator_Errors error, const std::string &description) {
    std::cout << "Generator: Error: ";
    if (error == Generator_Errors::ERROR_NO_USED_LABEL) {
        std::cout << "No used label " << description << std::endl;
    } else if (error == Generator_Errors::ERROR_OVER_USED_LABEL) {
        std::cout << "Overused label " << description << std::endl;
    } else if (error == Generator_Errors::ERROR_VALUE_DONT_LINK) {
        std::cout << "Value don`t link " << description << std::endl;
    } else if (error == Generator_Errors::ERROR_VALUE_IN_OTHER_STATE) {
        std::cout << "Value in other state " << description << std::endl;
    } else {
        std::cout << "Other Error" << std::endl;
    }
}