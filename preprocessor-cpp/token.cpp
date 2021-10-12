//
// Created by  Valentina Smirnova on 10/7/21.
//
#include <iostream>
#include "token.h"


MasterToken::MasterToken(const std::string &s) : s(s) {};

std::string MasterToken::get_substitution_str() { return this->s; }

ObjectLike::ObjectLike(const std::string &s) : MasterToken(s) {};

FunctionLike::FunctionLike(const std::string &s, const std::vector<std::vector<size_t>> &v) : MasterToken(s),
                                                                                              indexes(v) {};

std::string ObjectLike::substitute(const std::vector<std::string> &/*v*/) {
    return this->get_substitution_str();
}


std::string FunctionLike::substitute(const std::vector<std::string> &v) {
    std::cerr << "FunctionLike::substitute" << std::endl;
    std::cerr << "s template: " <<  get_substitution_str() << std::endl;
    for (auto d : v) {
        std::cerr << d << " ";
    }
    std::cerr << std::endl;
//  если хоть один из векторов индексов длина ноль - ошибка
    if (v.size() != this->indexes.size()) {
        std::cerr << "fucking throw " << std::endl;
        throw "invalid argument list";
    }

    std::vector<Ind> repls;
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < this->indexes[i].size(); ++j) {
            repls.emplace_back(Ind{this->indexes[i][j], v[i]});
        }
    }
    std::sort(repls.begin(), repls.end());

    const auto& str_temp = get_substitution_str();
    std::string ans;
    size_t last = 0;
    for (const auto &el : repls) {
//        std::cerr << "subs: " <<  get_substitution_str().substr(last, el.index) <<
//        "   index: " << el.index << "   repl: " << el.replacement  << "   last: " << last << std::endl;

        auto b = str_temp.substr(last, el.index - last);
//        std::cerr << "   last: " << last << "   index: " << el.index << "   substring: " << b
//                  << std::endl;
        ans += b + el.replacement;
//        ans += get_substitution_str().substr(last, el.index + shift) + el.replacement;
//        std::cerr << "ans: " << ans << std::endl;
        last = el.index;
    }
    ans += str_temp.substr(last, str_temp.length() - last);


    return ans;
}



