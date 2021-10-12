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

// TODO move to utils

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


std::pair<size_t, std::string> find_var(const size_t ind, const std::string &str) {
    std::string var;
    size_t i = ind;
    for (; i < str.length(); ++i) {
        if (str[i] == '\"') {
            ++i;
            while (str[i++] != '\"') {}
            continue;
        }
        if (str[i] == '\'') {
            ++i;
            while (str[i++] != '\'') {}
            continue;
        }

        if (is_name_part(str[i]) || (std::isdigit(str[i]) && !var.empty())) {
            var += str[i];
        } else {
            if (!var.empty()) {
                return {i, var};
            }
        }
    }

    return {i, var};
}

std::pair<size_t, std::vector<std::string>> find_args(const size_t ind, const std::string &str) {
    std::vector<std::string> args;
    size_t i = ind;
    if (str[i] == '(') {
        std::string loc;
        for (;;) {
            ++i;
//                            std::cerr << "i: " << i << std::endl;
            if (i >= str.length()) {
                std::cerr << "FUCK\n";
                break;
            }
//                            TODO what if no closing bracket?
            if (str[i] == ')') {
                ++i;
                args.push_back(loc);
                break;
            }
            if (str[i] == ',') {
                args.push_back(trim(loc));
                loc = "";
            } else {
                loc += str[i];
            }
        }
    }
    return {i, args};
}

std::string make_existing_replacement2(const std::string &line, const std::map<std::string, MasterToken *> &dict) {

//    std::cerr << "🧚🏼‍♀ line: ️" << line << std::endl;
    std::string ans;
    int shift = 0;
    for (size_t i = 0; i < line.length(); ++i) {
//        std::cerr << "i = " << i << ", el = " << line[i] << " : ";
        const auto &var = find_var(i, line);
//        std::cerr << "(var = " << var.second << ", i_f = " << var.first << ")\t";

        if (dict.find(var.second) != dict.end()) {
//                    std::cerr << "found var\n";
            const auto &args = find_args(var.first, line);
//                    std::cerr << "in substituting var: " << var << std::endl;
//            std::cerr << "args.size: " << args.second.size() << std::endl;
            ans += line.substr(i, var.first - var.second.length() - i) + dict.at(var.second)->substitute(args.second);

            i = args.first - 1;
//            std::cerr << "found\t upd: " << ans << std::endl;
        } else {
            ans += line.substr(i, var.first - i);
            i = var.first - 1;
//            std::cerr << "not found\t upd: " << ans << std::endl;
        }

    }


    return ans;
}

std::string make_existing_replacement(const std::string &line, const std::map<std::string, MasterToken *> &dict) {
    //    std::cout << "🧚🏼‍♀️" << line << std::endl;
    std::string var, ans;
    for (size_t i = 0; i < line.length(); ++i) {
//        std::cerr << "i = " << i << ", c = " << line[i] << std::endl;
        // skip string part of the code
        if (line[i] == '\"') {
            ans += line[i];
            ++i;
            while (line[i] != '\"') {
                ans += line[i++];
            }
            ans += line[i];
            continue;
        }
        if (line[i] == '\'') {
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
//                    std::cerr << "💮found var\n";
                    std::vector<std::string> args;
                    if (line[i] == '(') {
                        std::string loc;
//                        std::cerr << "💮found (\n";
                        for (;;) {
                            ++i;
//                            std::cerr << "i: " << i << std::endl;
                            if (i >= line.length()) {
                                std::cerr << "FUCK\n";
                                break;
                            }
//                            TODO what if no closing bracket?
                            if (line[i] == ')') {
                                ++i;
                                args.push_back(loc);
                                break;
                            }
                            if (line[i] == ',') {
                                args.push_back(trim(loc));
                                loc = "";
                            } else {
                                loc += line[i];
                            }
                        }
                    }
//                    std::cerr << "in substituting var: " << var << std::endl;
//                    std::cerr << "args.size: " << args.size() << std::endl;
                    ans += dict.at(var)->substitute(args);
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
//            std::cerr << "out substituting var: " << var << std::endl;
            ans += dict.at(var)->substitute({});
        } else {
            ans += var;
        }
//        std::cerr << "in ANS = " << ans << std::endl;
    }
    return ans;
}


void put_new_replacement(const std::string &identifier, std::istringstream &iss,
                         std::map<std::string, MasterToken *> &replacements) {

    std::string replacement;
    const auto &it = identifier.find('(');
    if (it == std::string::npos) { // macros is a simple object
        std::getline(iss >> std::ws, replacement);
//            TODO multiply lines
//            TODO change order of substitution
//            TODO remove useless variables
        auto s = make_existing_replacement2(replacement, replacements);

//        std::cerr << identifier << " | " << s << std::endl;

        replacements[identifier] = new ObjectLike{s};
    } else { // macros is a function
        std::string params_str;
        const auto &jt = identifier.find(')');

        if (jt == std::string::npos) {
            std::getline(iss, params_str, ')');
            params_str = identifier.substr(it + 1, identifier.length() - it) + params_str;
        } else {
            params_str = identifier.substr(it + 1, jt - it - 1);
        }
        std::cerr << "before removing: " << params_str << std::endl;
        params_str.erase(remove_if(params_str.begin(), params_str.end(), isspace), params_str.end());
        std::cerr << "after removing: " << params_str << std::endl;

        std::istringstream jss(params_str);
        std::vector<std::string> params;
        std::string par;
        while (std::getline(jss, par, ',')) {
            std::cerr << "\tpar: " << par;
            params.push_back(par);
        }
        std::cerr << std::endl;

        std::vector<std::vector<size_t>> idxs(params.size(), std::vector<size_t>());
        std::getline(iss >> std::ws, replacement);

        int shift = 0;
        std::string replacement_upd;
        std::cerr << "\nreplacement = " << replacement << "\nreplacement = 0123456789" << std::endl;
        for (size_t i = 0; i < replacement.length(); ++i) {
            std::cerr << "i = " << i << ", el = " << replacement[i] << " : ";
            const auto &var = find_var(i, replacement);
            std::cerr << "(var = " << var.second << ", i_f = " << var.first << ")\t";

            const auto &j = std::find(params.begin(), params.end(), var.second);
            if (j != params.end()) {
                auto index = std::distance(params.begin(), j);
                shift += var.second.length();
                idxs[index].push_back(var.first - shift);
                replacement_upd += replacement.substr(i, var.first - var.second.length() - i);
                std::cerr << "found\t upd: " << replacement_upd << std::endl;
            } else {
                replacement_upd += replacement.substr(i, var.first - i);
                std::cerr << "not found\t upd: " << replacement_upd << std::endl;
            }

            i = var.first - 1;
        }
        std::cerr << "old replacement : " << replacement << ", upd replacement : " << replacement_upd << std::endl;


        replacements[identifier.substr(0, it)] = new FunctionLike{
                make_existing_replacement2(replacement_upd, replacements), idxs};
    }

}

void read_struct() {
    std::fstream fs;
    fs.open("../data/hello.cpp", std::fstream::in);

    std::string line;
    std::map<std::string, MasterToken *> replacements;
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
//         TODO undefine
        if (p1 == "#define") {
            put_new_replacement(identifier, iss, replacements);
        } else if (p1 == "#" && identifier == "define") {
            iss >> identifier;
            put_new_replacement(identifier, iss, replacements);
        } else {
            std::cout << make_existing_replacement2(line, replacements) << std::endl;
            put_line(line);
        }
    }
    fs.close();
}

int main() {
    read_struct();
//    auto pa = find_var(6, "int u = FUNC(1,2);");
//    std::cerr << pa.first << " | " << pa.second << std::endl;
//
//    auto ar = find_args(12, "int u = FUNC(\"1kj\",2);");
//    std::cerr << ar.first << " | " << ar.second[0] << std::endl;

//    std::string ff = "   \"fkd(pos) mf fjnfs + jfj\"   ";
//    std::cerr << trim(ff) << "|" << std::endl;

//    std::string rt = "fkd(pos) mf fjnfs + jfj";
//    std::vector<std::string> gh{"pos", "fj", "m"};
////    update_function_replacement(rt, gh);
//    std::cerr << rt << std::endl;
//
//    auto fj = find_all_indexes(rt, "fj");
//    for (auto g : fj) {
//        std::cerr << g << " ";
//    }

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


//    std::map<std::string, MasterToken*> m;
//    m["PN"] = new ObjectLike{"100"};
//    m["SUM"] = new ObjectLike{"3"};
//    std::string s = "    int i, SUM = 0;";
//    std::cout << make_existing_replacement(s, m) << std::endl;
//    std::cout << make_existing_replacement2(s, m) << std::endl;

    return 0;
}