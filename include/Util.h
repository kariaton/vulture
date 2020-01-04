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
        void formatCandles(std::string &, std::vector<std::vector<double>> &) const;
        void getConfigFile(std::string &) const;

   private:
        void _clearOCHL(std::vector<std::vector<double>> &) const;
        void _reverseOCHL(std::vector<std::vector<double>> &) const;
};

#endif // UTIL_H
