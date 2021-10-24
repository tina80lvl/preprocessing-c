#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include "token.h"
#include "utils.h"


std::pair<size_t, std::string>
find_var(const size_t ind, const std::string &str) {
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
std::string prv(const std::vector<std::string> &v) {
    if (v.empty()) return "";
    std::string ans = "(";
    for (const auto &el : v) {
        ans += el + ',';
    }
    ans.pop_back();
    return ans + ')';
}

std::pair<size_t, std::vector<std::string>>
find_args(const size_t ind, const std::string &str) {
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
//                    std::cerr << "arg: " << trim(loc) << "\t";
                    args.push_back(trim(loc));
                    break;
                } else {
                    --cnt;
                }
            }
            if (str[i] == ',' && cnt <= 1) {
                std::cerr << "arg: " << trim(loc) << "\t";
                args.push_back(trim(loc));
                loc = "";
            } else {
                loc += str[i];
            }
        }
    }
    std::cerr << "\nreturned args.size() = " << args.size();
    std::cerr << "\nreturned args: " << prv(args) << "\n\n";
    return {i, args};
}

int cnt_glob = 0;
int lvl = 0;

void prnt(const std::string &name, const std::map<std::string, bool> &m) {
    std::cerr << name << " map elements: ";
    for (auto el : m) {
        std::cerr << "key: " << el.first << ", val: " << el.second << ";\t";
    }
    std::cerr << std::endl;

}
void prnt(const std::map<std::string, MasterToken *> &dict) {
    std::cerr << "PRINTING DICT\n";
    for (auto el : dict) {
        std::cerr << "key:\t" << el.first << "\t\tval:\t" << el.second->get_substitution_str() << "\n";
    }
    std::cerr << std::endl;

}

std::string make_existing_replacement_v2(const std::string &line,
                                         const std::map<std::string, MasterToken *> &dict) {
    std::cerr << "\n\n🛑 START RCURSION LEVEL: " << ++lvl << "\n";
    std::cerr << "line: " << line << "\n";
    std::string ans;
    for (size_t i = 0; i < line.length(); ++i) {
        const auto &var = find_var(i, line);
        std::cerr << "var: " << var.second << "\n";
        if (dict.find(var.second) != dict.end()/* && !ignored[var.second]*/) {
            std::pair<size_t, std::vector<std::string>> args;
            args = find_args(var.first, line);

            try {
                std::cerr << "trying to subs " << var.second << "\n";
                ans += line.substr(i, var.first - var.second.length() - i) +
                       dict.at(var.second)->substitute(args.second);
            } catch (const std::exception &e) {
                std::cerr << "fail " << var.second << "\n";
                std::cerr << e.what() << std::endl;
                ans += line.substr(i, args.first - i);
            }
            i = args.first - 1;

        } else {
            ans += line.substr(i, var.first - i);
            i = var.first - 1;
        }
//        std::cerr << "i: " << i << "\t loc ans: " << ans << "\n";
    }
    std::cerr << "\n\n❎ END RCURSION LEVEL: " << lvl-- << "\n";
    return ans;
}

std::string make_existing_replacement_v3(const std::string &line,
                                       const std::map<std::string, MasterToken *> &dict) {
    std::string loc_line = line;
    for (const auto& macro : dict) {
        std::string ans;
        for (size_t i = 0; i < loc_line.length(); ++i) {
            const auto &var = find_var(i, loc_line);
            if (var.second == macro.first) {
                const auto & args = find_args(var.first, loc_line);

                try {
                    ans += loc_line.substr(i, var.first - var.second.length() - i) +
                           dict.at(var.second)->substitute(args.second);
                } catch (const std::exception &e) {
                    std::cerr << e.what() << std::endl;
                    ans += loc_line.substr(i, args.first - i);
                }
                i = args.first - 1;

            } else {
                ans += loc_line.substr(i, var.first - i);
                i = var.first - 1;
            }
        }
        loc_line = ans;
    }

    return loc_line;
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
                         std::map<std::string, MasterToken *> &replacements,
                         std::map<std::string, std::string> &recursive_replacement) {
    std::string replacement;
    const auto &it = identifier.find('(');
    if (it == std::string::npos) { // macros is a simple object
        std::getline(iss >> std::ws, replacement);
        replacement = trim(replacement);
//        std::cerr << "🈶replaced var before: " << replacement << "\n";
        for (size_t i = 0; i < replacement.length(); ++i) {
            const auto &var = find_var(i, replacement);
            i = var.first - 1;
            if (!var.second.empty() && var.second == identifier) {
                replacement.insert(var.first, "_id");
                recursive_replacement[var.second + "_id"] = var.second;
                i += 3;
            }
        }
//        std::cerr << "🈶replaced var after: " << replacement << "\n";
        replacements[identifier] = new ObjectLike{replacement};
    } else { // macros is a function
        std::string params_str;
        const auto &jt = identifier.find(')');

        if (jt == std::string::npos) {
            std::getline(iss, params_str, ')');
            params_str = identifier.substr(it + 1, identifier.length() - it) +
                         params_str;
        } else {
            params_str = identifier.substr(it + 1, jt - it - 1);
        }
        params_str.erase(
                remove_if(params_str.begin(), params_str.end(), isspace),
                params_str.end());

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

        std::vector<std::vector<size_t>> idxs(params.size(),
                                              std::vector<size_t>());
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
                replacement_upd += replacement.substr(i, var.first -
                                                         var.second.length() -
                                                         i);
            } else {
                replacement_upd += replacement.substr(i, var.first - i);
                if (var.second == identifier.substr(0, it)) {
                    recursive_replacement[var.second + "_id"] = var.second;
//                    replacements[var.second + "_id"] = new ObjectLike{var.second};
                    replacement_upd += "_id";
                    shift -= 3;
                }
            }

            i = var.first - 1;
        }
//        for (const auto &vec : idxs) { // checking all the parameters are used
//            if (vec.empty()) return;
//        }
        std::cerr << "!!!!!!!!! idxs.size() = " << idxs.size() << std::endl;

        replacements[identifier.substr(0, it)] = new FunctionLike{
                replacement_upd, idxs, va_flag};
    }

}


std::string scan_for_recursive(const std::string& line,
        const std::map<std::string, std::string>& recursive_replacements) {
//    std::cerr << "RECURSIVE SCANNING line: \'" << line << "\'\n";
    std::string ans;
    for (size_t i = 0; i < line.size(); ++i) {
        const auto& var = find_var(i, line);
        ans += line.substr(i, var.first - var.second.length() - i);
//        std::cerr << "var: " << var.second << "\t i_f: " << var.first << "\t i_c: " << i << "\n";
//        std::cerr << "0 loc ans: " << ans << "\n";
        if (recursive_replacements.find(var.second) != recursive_replacements.end()) {
            ans += recursive_replacements.at(var.second);
//            std::cerr << "1 loc ans: " << ans << "\n";
        } else {
            ans += var.second;
//            ans += line.substr(i, var.first - i);
//            std::cerr << "2 loc ans: " << ans << "\n";
        }
        i = var.first - 1;
    }
    return ans;
}

void read_struct(std::fstream &fs_in, std::fstream &fs_out) {
    std::string line;
    std::map<std::string, MasterToken *> replacements;
    std::map<std::string, std::string> recursive_replacements;

    while (std::getline(fs_in, line)) {
        if (line.empty()) {
            put_line(fs_out);
            continue;
        }
        check_multiply_lines(fs_in, line);
        if (ltrim(line).substr(0, 2) == "//") {
            put_line(fs_out, line);
            continue;
        }

        std::istringstream iss(line);
        std::string p1;
        iss >> p1;

        std::string identifier;
        iss >> identifier;
        if (p1 == "#define") {
            put_new_replacement(identifier, iss, replacements, recursive_replacements);
        } else if (p1 == "#undef") {
            replacements.erase(identifier);
        } else if (p1 == "#" && identifier == "define") {
            iss >> identifier;
            put_new_replacement(identifier, iss, replacements, recursive_replacements);
        } else if (p1 == "#" && identifier == "undef") {
            iss >> identifier;
            replacements.erase(identifier);
        } else {
            prnt(replacements);
            std::map<std::string, bool> ignored;
            std::string repl1 = line, repl2 = make_existing_replacement_v2(line,
                                                                           replacements);
            while (repl1 != repl2) {
                std::cerr << "outer local\nrepl1: " << repl1 << "\nrepl2: " << repl2 << "\n";
                repl1 = repl2;
                repl2 = make_existing_replacement_v2(repl2, replacements);
            }
            std::cerr << "outer final\nrepl1: " << repl1 << "\nrepl2: " << repl2 << "\n";
//            put_line(fs_out, repl2);
            put_line(fs_out, scan_for_recursive(repl2, recursive_replacements));
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
//    std::string name = "brackets.c";
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
    std::string name = "spec_example_5.c";
//    std::string name = "variable.c";
//    std::string name = "undef.c";
//    std::string name = "crossing.c";
    std::cout << "Preprocessing file " << name << std::endl;
    std::fstream fs_in;
    fs_in.open("../data/originals/" + name, std::fstream::in);

    std::fstream fs_out;
    std::string out_path = "../data/preprocessed/" + name;
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
