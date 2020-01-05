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

        // CONFIG
        void getConfigFile(std::string &) const;
        void getBTXParam(const char *, std::string &, std::string &) const;
        void getDbParam(const char *, std::string &, std::string &, std::string &, std::string &) const;

        // CANDLES
        void formatCandles(std::string &, std::vector<std::vector<double>> &) const;

   private:
        void _clearOCHL(std::vector<std::vector<double>> &) const;
        void _reverseOCHL(std::vector<std::vector<double>> &) const;
};

#endif // UTIL_H
