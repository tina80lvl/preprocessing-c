//
// Created by  Valentina Smirnova on 10/7/21.
//
#include "token.h"

MasterToken::MasterToken(const std::string &s) : s(s) {};

std::string MasterToken::get_substitution_str() { return this->s; }

ObjectLike::ObjectLike(const std::string &s) : MasterToken(s) {};

FunctionLike::FunctionLike(const std::string &s, const std::vector<std::vector<size_t>> &v, const bool &va)
        : MasterToken(s),
          indexes(v), various_args(va) {};

std::string vec_to_str(const std::vector<std::string> &v) {
    if (v.empty()) return "";
    std::string ans = "(";
    for (const auto &el : v) {
        ans += el + ',';
    }
    ans.pop_back();
    return ans + ')';
}

std::string ObjectLike::substitute(const std::vector<std::string> &v) {
    return this->get_substitution_str() + vec_to_str(v);
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
        std::string add;
        for (size_t i = (size_t) std::distance(v.begin(), last_it); i < v.size(); ++i) {
            add += ',' + v[i];
        }
        nv[nv.size() - 1] += add;
    }

    std::vector<Ind> repls;
    for (size_t i = 0; i < this->indexes.size(); ++i) {
        for (size_t j = 0; j < this->indexes[i].size(); ++j) {
            repls.emplace_back(Ind{this->indexes[i][j], nv[i]});
        }
    }
    std::sort(repls.begin(), repls.end());

    const auto &str_temp = get_substitution_str();
    std::string ans;
    size_t last = 0;
    for (const auto &el : repls) {
        ans += str_temp.substr(last, el.index - last) + el.replacement;
        last = el.index;
    }
    ans += str_temp.substr(last, str_temp.length() - last);

    std::cerr << "ans: " << ans << "\n";
    return ans;
}



