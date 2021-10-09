//
// Created by  Valentina Smirnova on 10/7/21.
//

#ifndef PREPROCESSOR_CPP_TOKEN_H
#define PREPROCESSOR_CPP_TOKEN_H

#include <string>
#include <map>
#include <vector>

struct Ind {
    size_t index;
    std::string replacement;

    bool operator <(const Ind & ind) const
    {
        return index < ind.index;
    }
};

class MasterToken {
public:
    MasterToken(const std::string &s);
    std::string get_substitution_str();
    virtual std::string substitute(const std::vector<std::string>& v) = 0;
private:
    std::string s;
};

class ObjectLike : public MasterToken {
public:
    ObjectLike (const std::string& s);
    std::string substitute(const std::vector<std::string>& /*v*/) override;
};


class FunctionLike : public MasterToken {
public:
//    TODO make indexes private
    std::vector<std::vector<size_t>> indexes;
    FunctionLike (const std::string& s, const std::vector<std::vector<size_t>>& v);
    std::string substitute(const std::vector<std::string>& v) override;
};



#endif //PREPROCESSOR_CPP_TOKEN_H
