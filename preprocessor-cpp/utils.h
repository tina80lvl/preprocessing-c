//
// Created by  Valentina Smirnova on 10/13/21.
//

#ifndef PREPROCESSOR_CPP_UTILS_H
#define PREPROCESSOR_CPP_UTILS_H

#include <cstring>
#include <fstream>

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

void put_line(std::fstream &fs_out, const std::string &line = "") {
    fs_out << line << std::endl;
}

bool is_name_part(const char ch) {
    return isalpha(ch) || ch == '_';
}


#endif //PREPROCESSOR_CPP_UTILS_H
