#include <iostream>
#include <algorithm>
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

std::string make_replacement(const std::string &line, const std::map<std::string, MasterToken*> &dict) {
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
//                    ans += dict.at(var);
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
//            ans += dict.at(var);
        } else {
            ans += var;
        }
//        std::cerr << "in ANS = " << ans << std::endl;
    }
    return ans;
}


std::vector<size_t> find_all_indexes(const std::string& str, const std::string& substr) {
    std::vector<size_t> indexes;
//    TODO if other name is a substring of another var
    size_t pos = str.find(substr, 0);
    while(pos != std::string::npos)
    {
        indexes.push_back(pos);
        pos = str.find(substr, pos + 1);
    }
    if (indexes.empty()) {
        throw "function parameter absent if function body";
    }
    return indexes;
}

void update_function_replacement(std::string& repl, const std::vector<std::string>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        auto pos = repl.find(params[i]);
        while(pos != std::string::npos)
        {
            repl.erase(pos, params[i].length());
            pos = repl.find(params[i], pos);
        }
    }
}

void put_replacement(const std::string &identifier, std::istringstream &iss,
                     std::map<std::string, MasterToken*> &replacements) {

    std::string replacement;
    const auto& it = identifier.find('(');
    if (it == std::string::npos) { // macros is a simple object
        std::getline(iss >> std::ws, replacement);
//            TODO multiply lines
//            TODO change order of substitution
//            TODO remove useless variables
        auto s = make_replacement(replacement, replacements);
        std::cerr << identifier << " | " << s << std::endl;
        replacements[identifier] = new ObjectLike{s};
    } else { // macros is a function
        std::string params_str;
        const auto& jt = identifier.find(')');

        if (jt == std::string::npos) {
            std::getline(iss, params_str, ')');
            params_str = identifier.substr(it, identifier.length() - it) + params_str;
        } else {
            params_str = identifier.substr(it, jt - it);
        }
        remove_if(params_str.begin(), params_str.end(), isspace);

        std::istringstream jss(params_str);
        std::vector<std::string> params;
        std::string par;
        while (std::getline(jss, par, ',')) {
            params.push_back(par);
        }

        std::vector<std::vector<size_t>> idxs(params.size(), std::vector<size_t>());
        std::getline(iss >> std::ws, replacement);
        for (size_t i = 0; i < params.size(); ++i) {
            idxs[i] = find_all_indexes(replacement, params[i]);
        }

        update_function_replacement(replacement, params);

        replacements[identifier.substr(0, it)] = new FunctionLike{replacement, idxs};
    }

}

void read_struct() {
    std::fstream fs;
    fs.open("../data/hello.cpp", std::fstream::in);

    std::string line;
    std::map<std::string, MasterToken*> replacements;
    while (std::getline(fs, line)) {
        if (line.empty()) {
            put_line();
            continue;
        }

        std::istringstream iss(line);
        std::string p1;
        iss >> p1;


        std::string identifier;
        iss >> identifier;
        if (p1 == "#define") {
            put_replacement(identifier, iss, replacements);
        } else if (p1 == "#" && identifier == "define") {
            iss >> identifier;
            put_replacement(identifier, iss, replacements);
        } else {
            std::cout << make_replacement(line, replacements) << std::endl;
            put_line(line);
        }
    }
    fs.close();
}

int main() {
//    read_struct();

    std::string rt = "fkd(pos) mf fjnfs + jfj";
    std::vector<std::string> gh{"pos", "fj", "m"};
//    update_function_replacement(rt, gh);
    std::cerr << rt << std::endl;

    auto fj = find_all_indexes(rt, "fj");
    for (auto g : fj) {
        std::cerr << g << " ";
    }

//    std::string loc, s = "  dk,d fd,f ) fkf";
//    std::istringstream iss(s);
//    std::cerr << s << std::endl;
//    while (std::getline(iss >> std::ws, loc, ')')) {
//        std::cerr << loc << std::endl;
//    }



//    MasterToken* o_l = new ObjectLike{"obj string"};

//
//    std::cout << o_l->substitute({}) << std::endl;
//
//    std::vector<std::vector<size_t>> v(2, std::vector<size_t> ());
//    v[0].push_back(1);
//    v[1].push_back(5);
//    v[1].push_back(6);
//    v[1].push_back(11);
//
//    MasterToken* f_l = new FunctionLike{"0123456789abcdefg", v};
//
//    std::cout << f_l->substitute({"++","-"}) << std::endl;


//    std::map<std::string, std::string> m;
//    m["PN"] = "100";
//    m["SUM"] = "3";
//    std::string s = "    int i, sum = 0;";
//    std::cout << make_replacement(s, m) << std::endl;

    return 0;
}