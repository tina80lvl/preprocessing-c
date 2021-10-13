//
// Created by  Valentina Smirnova on 10/7/21.
//
#include <iostream>
#include "token.h"

MasterToken::MasterToken(const std::string &s) : s(s) {};

std::string MasterToken::get_substitution_str() { return this->s; }

ObjectLike::ObjectLike(const std::string &s) : MasterToken(s) {};

FunctionLike::FunctionLike(const std::string &s, const std::vector<std::vector<size_t>> &v, const bool va) : MasterToken(s),
                                                                                              indexes(v), various_args(va) {};

std::string ObjectLike::substitute(const std::vector<std::string> &/*v*/) {
    return this->get_substitution_str();
}

std::string concat_va(const size_t& ind, const std::vector<std::string> &v) {
    std::string ans;
    for (size_t i = ind; i < v.size(); ++i) {
        ans += ',' + v[i];
    }
    return ans;
}

std::string FunctionLike::substitute(const std::vector<std::string> &v) {
    if (v.size() < this->indexes.size()) {
        throw std::invalid_argument("ERROR: Too little arguments in FunctionLike::substitute.");
    }

    if (v.size() != this->indexes.size() && !this->various_args) {
        throw std::invalid_argument("ERROR: Invalid argument list in FunctionLike::substitute.");
    }

    std::vector<std::string>::const_iterator first_it = v.begin();
    std::vector<std::string>::const_iterator last_it = v.begin() + this->indexes.size();
    std::vector<std::string> nv(first_it, last_it);
    if (v.size() > this->indexes.size()) {
        nv[nv.size() - 1] += concat_va((size_t)std::distance(v.begin(), last_it), v);
        std::cerr << "❌" << nv[nv.size() - 1];
    }

    std::vector<Ind> repls;
    for (size_t i = 0; i <  this->indexes.size(); ++i) {
        for (size_t j = 0; j < this->indexes[i].size(); ++j) {
            repls.emplace_back(Ind{this->indexes[i][j], nv[i]});
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



