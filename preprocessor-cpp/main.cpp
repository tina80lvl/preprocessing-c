#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include "token.h"
#include "utils.h"


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
                args.push_back(trim(loc));
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
            try {
                std::string l = make_existing_replacement2(dict.at(var.second)->substitute(args.second), dict);
//                std::string l = dict.at(var.second)->substitute(args.second);
                ans += line.substr(i, var.first - var.second.length() - i) + l;
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                ans += line.substr(i, args.first - i);
            }

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


void check_multiply_lines(std::fstream &iss, std::string& line) {
    std::cerr << "relp: \"" << line << "\"\n";
    while (trim(line)[line.length() - 1] == '\\') {
        std::cerr << "WHILE\t";
        line.pop_back();
        std::string new_line;
        std::getline(iss, new_line);
        std::cerr << "new_line: \"" << new_line << "\"\n";
        line += trim(new_line);
        std::cerr << "\treplacement: " << line << std::endl;
    }
};


void put_new_replacement(const std::string &identifier, std::istringstream &iss,
                         std::map<std::string, MasterToken *> &replacements) {

    std::string replacement;
    const auto &it = identifier.find('(');
    if (it == std::string::npos) { // macros is a simple object
        std::getline(iss >> std::ws, replacement);

//        std::cerr << identifier << " | " << s << std::endl;

//        replacements[identifier] = new ObjectLike{s};
        replacements[identifier] = new ObjectLike{replacement};
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

        replacements[identifier.substr(0, it)] = new FunctionLike{replacement_upd, idxs};
    }

}

void read_struct(std::fstream& fs_in, std::fstream& fs_out) {
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
//            std::cout << make_existing_replacement2(line, replacements) << std::endl;
            put_line(fs_out, make_existing_replacement2(line, replacements));
        }
    }
}

int main() {
    for (const auto & entry : std::filesystem::directory_iterator("../data/originals")) {
        std::cout << entry.path() << std::endl;
        std::fstream fs_in;
        fs_in.open(entry.path(), std::fstream::in);
        std::fstream fs_out;
        std::string out_path = entry.path().generic_string().replace(8, 9, "preprocessed");
        std::cout << out_path << std::endl;
        fs_out.open(out_path, std::fstream::out);
        read_struct(fs_in, fs_out);

        fs_in.close();
        fs_out.close();
    }

    return 0;
}
