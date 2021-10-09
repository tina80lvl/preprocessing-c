#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "token.h"

// skip_whitespace
// parse_define
// parse_name
// parse_var
// parse_fun
// substitute_args

void put_line(const std::string &line = "") {

}

bool is_name_part(const char ch) {
    return isalpha(ch) || ch == '_';
}

std::string make_replacement(const std::string &line, const std::map<std::string, std::string> &dict) {
//    std::cout << "🧚🏼‍♀️" << line << std::endl;
    std::string var, ans;
    for (size_t i = 0; i < line.length(); ++i) {
//        std::cerr << "i = " << i << ", c = " << line[i] << std::endl;
        // skip string part of the code
        if (line[i] == '\"') {
//            std::cerr << "skip"  << std::endl;
            ans += line[i];
            ++i;
            while (line[i] != '\"') {
                ans += line[i++];
            }
            ans += line[i];
            continue;
        }
        if (line[i] == '\'') {
//            std::cerr << "skip"  << std::endl;
            ans += line[i];
            ++i;
            while (line[i] != '\'') {
                ans += line[i++];
            }
            ans += line[i];
            continue;
        }

        if (is_name_part(line[i]) || (std::isdigit(line[i]) && is_name_part(line[i - 1]))) {
//            std::cerr << "name part"  << std::endl;
            var += line[i];
        } else {
//            std::cerr << "not name part"  << std::endl;
            if (!var.empty()) {
//                std::cerr << "var not empty"  << std::endl;
                if (dict.find(var) != dict.end()) {
                    // TODO if function
                    ans += dict.at(var);
                } else {
                    ans += var;
                }
//                std::cerr << "in ANS = " << ans << std::endl;
                var = "";
            }

            ans += line[i];
//            std::cerr << "out ANS = " << ans << std::endl;
        }

    }
    if (!var.empty()) {
//        std::cerr << "var not empty"  << std::endl;
        if (dict.find(var) != dict.end()) {
            ans += dict.at(var);
        } else {
            ans += var;
        }
//        std::cerr << "in ANS = " << ans << std::endl;
    }
    return ans;
}

//std::pair<std::string, std::vector<std::string>>
//separate_function_name(std::string &identifier, std::string &replacement) {
//
//}

void put_replacement(const std::string &identifier, std::string &replacement, std::istringstream &iss,
                     std::map<std::string, std::string> &replacements) {
    std::getline(iss >> std::ws, replacement);
//            TODO support functions
//            TODO multiply lines
//            TODO change order of substitution
    auto s = make_replacement(replacement, replacements);
    std::cerr << identifier << " | " << s << std::endl;
    replacements[identifier] = s;
}

void read_struct() {
    std::fstream fs;
    fs.open("../hello.cpp", std::fstream::in);

    std::string line;
    std::map<std::string, std::string> replacements;
    while (std::getline(fs, line)) {
        if (line.empty()) {
            put_line();
            continue;
        }

        std::istringstream iss(line);
        std::string p1;
        iss >> p1;


        std::string identifier, replacement;
        iss >> identifier;
        if (p1 == "#define") {
            put_replacement(identifier, replacement, iss, replacements);
        } else if (p1 == "#" && identifier == "define") {
            iss >> identifier;
            put_replacement(identifier, replacement, iss, replacements);
        } else {
            std::cout << make_replacement(line, replacements) << std::endl;
            put_line(line);
        }
    }
    fs.close();
}

int main() {
//    read_struct();


    MasterToken* o_l = new ObjectLike{"obj string"};

    std::cout << o_l->substitute({}) << std::endl;

    std::vector<std::vector<size_t>> v(2, std::vector<size_t> ());
    v[0].push_back(1);
    v[1].push_back(5);
    v[1].push_back(6);
    v[1].push_back(11);

    MasterToken* f_l = new FunctionLike{"0123456789abcdefg", v};

    std::cout << f_l->substitute({"++","-"}) << std::endl;


//    std::map<std::string, std::string> m;
//    m["PN"] = "100";
//    m["SUM"] = "3";
//    std::string s = "    int i, sum = 0;";
//    std::cout << make_replacement(s, m) << std::endl;

    return 0;
}