#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include "token.h"
#include "utils.h"


std::pair<size_t, std::string> find_var(const size_t ind, const std::string &str) {
    std::string var;
    size_t i = ind;

    for (; i < str.length(); ++i) {
        if (str[i] == '\"') {
            ++i;
            while (str[i++] != '\"' && i < str.length()) {}
            continue;
        }
        if (str[i] == '\'') {
            ++i;
            while (str[i++] != '\'' && i < str.length()) {}
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
    std::cerr << "\nfind_args\n";
    std::vector<std::string> args;
    size_t i = ind;
    if (str[i] == '(') {
        int cnt = 1;
        std::string loc;
        for (;;) {
            ++i;
            if (str[i] == '(') ++cnt;
            if (i >= str.length()) {
                break;
            }
//          TODO what if no closing bracket?
            if (str[i] == ')') {
                if (cnt == 1) {
                    ++i;
                    std::cerr << "arg: " << trim(loc) << "\t";
                    args.push_back(trim(loc));
                    break;
                } else {
                    --cnt;
                }
            }
            if (str[i] == ',') {
                std::cerr << "arg: " << trim(loc) << "\t";
                args.push_back(trim(loc));
                loc = "";
            } else {
                loc += str[i];
            }
        }
    }
    return {i, args};
}


std::string make_existing_replacement2(const std::string &line, const std::map<std::string, MasterToken *> &dict, std::map<std::string, bool>& visited) {
    std::cerr << "\nmake_existing_replacement2\n";
    std::cerr << "line: " << line << "\n";
    std::string ans;
    for (size_t i = 0; i < line.length(); ++i) {
        const auto &var = find_var(i, line);
        std::cerr << "var: " << var.second << "\n";
        if (dict.find(var.second) != dict.end() && !visited[var.second]) {
            visited[var.second] = true;
            auto args = find_args(var.first, line);
            for (auto arg : args.second) {
//                visited.erase(var.second);
                auto y = make_existing_replacement2(arg, dict, visited);
                std::cerr << "arg before: \'" << arg << "\'\t"<< "arg after: \'" << y  << "\'\n";
                arg = y;
            }
            try {
                ans += line.substr(i, var.first - var.second.length() - i) +
                       make_existing_replacement2(dict.at(var.second)->substitute(args.second), dict, visited);
                visited.erase(var.second);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
                ans += line.substr(i, args.first - i);
            }
            i = args.first - 1;
        } else {
            ans += line.substr(i, var.first - i);
            i = var.first - 1;
        }
    }

    return ans;
}


void check_multiply_lines(std::fstream &iss, std::string &line) {
    std::string new_line;
    while (trim(line).back() == '\\') {
        line.pop_back();
        std::getline(iss, new_line);
        line += trim(new_line);
    }
}


void put_new_replacement(const std::string &identifier, std::istringstream &iss,
                         std::map<std::string, MasterToken *> &replacements) {
    std::string replacement;
    const auto &it = identifier.find('(');
    if (it == std::string::npos) { // macros is a simple object
        std::getline(iss >> std::ws, replacement);

        replacements[identifier] = new ObjectLike{trim(replacement)};
    } else { // macros is a function
        std::string params_str;
        const auto &jt = identifier.find(')');

        if (jt == std::string::npos) {
            std::getline(iss, params_str, ')');
            params_str = identifier.substr(it + 1, identifier.length() - it) + params_str;
        } else {
            params_str = identifier.substr(it + 1, jt - it - 1);
        }
        params_str.erase(remove_if(params_str.begin(), params_str.end(), isspace), params_str.end());

        std::istringstream jss(params_str);
        std::vector<std::string> params;
        std::string par;
        bool va_flag = false;
        while (std::getline(jss, par, ',')) {
            if (par == "...") {
                if (va_flag) {
                    return;
                }
                va_flag = true;
                params.emplace_back("__VA_ARGS__");
            } else {
                params.push_back(par);
            }
        }

        if (va_flag && (params[params.size() - 1] != "__VA_ARGS__")) {
            return;
        }

        std::vector<std::vector<size_t>> idxs(params.size(), std::vector<size_t>());
        std::getline(iss >> std::ws, replacement);

        int shift = 0;
        std::string replacement_upd;
        for (size_t i = 0; i < replacement.length(); ++i) {
            const auto &var = find_var(i, replacement);

            const auto &j = std::find(params.begin(), params.end(), var.second);
            if (j != params.end()) {
                auto index = std::distance(params.begin(), j);
                shift += var.second.length();
                idxs[index].push_back(var.first - shift);
                replacement_upd += replacement.substr(i, var.first - var.second.length() - i);
            } else {
                replacement_upd += replacement.substr(i, var.first - i);
            }

            i = var.first - 1;
        }
        for (const auto &vec : idxs) { // checking all the parameters are used
            if (vec.empty()) return;
        }

        replacements[identifier.substr(0, it)] = new FunctionLike{replacement_upd, idxs, va_flag};
    }

}

void read_struct(std::fstream &fs_in, std::fstream &fs_out) {
    std::string line;
    std::map<std::string, MasterToken *> replacements;
    while (std::getline(fs_in, line)) {
        if (line.empty()) {
            put_line(fs_out);
            continue;
        }
        check_multiply_lines(fs_in, line);

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
//            put_line(fs_out, make_existing_replacement2(make_existing_replacement2(line, replacements), replacements));
            std::map<std::string, bool> vis;
            put_line(fs_out, make_existing_replacement2(line, replacements, vis));
        }
    }
}

int main() {

//    for (const auto &entry : std::filesystem::directory_iterator("../data/originals")) {
//        std::cout << "Preprocessing file " << entry.path() << std::endl;
//        std::fstream fs_in;
//        fs_in.open(entry.path(), std::fstream::in);
//
//        std::fstream fs_out;
//        std::string out_path = entry.path().generic_string().replace(8, 9, "preprocessed");
//        fs_out.open(out_path, std::fstream::out);
//
//
//        auto t1 = std::chrono::high_resolution_clock::now();
//        read_struct(fs_in, fs_out);
//        auto t2 = std::chrono::high_resolution_clock::now();
//
//        std::chrono::duration<double, std::milli> ms_double = t2 - t1;
//        std::cout << "Finished in " << ms_double.count() << " ms" << std::endl;
//
//        fs_in.close();
//        fs_out.close();
//    }

//    std::string name = "alias.c";
//    std::string name = "function.c";
//    std::string name = "in_main.c";
//    std::string name = "multiline.c";
//    std::string name = "order.c";
//    std::string name = "out_of_range.c"; //len
//    std::string name = "recursive_function.c";
//    std::string name = "recursive_variable.c";
//    std::string name = "redefinition.c";
//    std::string name = "spaces.c";
//    std::string name = "spec_example_1.c";
//    std::string name = "spec_example_5.c";
//    std::string name = "variable.c";
    std::string name = "brackets.c";
    std::cout << "Preprocessing file " << name << std::endl;
    std::fstream fs_in;
    fs_in.open("../data/originals/"+name, std::fstream::in);

    std::fstream fs_out;
    std::string out_path = "../data/preprocessed/"+name;
    fs_out.open(out_path, std::fstream::out);


    auto t1 = std::chrono::high_resolution_clock::now();
    read_struct(fs_in, fs_out);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "Finished in " << ms_double.count() << " ms" << std::endl;

    fs_in.close();
    fs_out.close();

    return 0;
}
