#ifndef UTIL_H
#define UTIL_H

#include <rapidjson/document.h>
#include <string>
#include <vector>

class Util
{
    public:
        Util();
        virtual ~Util();
        void getBTXParam(const char*, std::string &, std::string &) const;
        void macd(std::string &, std::vector<double> &) const;
        void getConfigFile(std::string &) const;
};

#endif // UTIL_H
