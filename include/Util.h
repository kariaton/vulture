#ifndef UTIL_H
#define UTIL_H

#include <rapidjson/document.h>
#include <string>
#include <vector>
#include <memory>

#include "Order.h"

class Util
{
    public:
        Util();
        virtual ~Util();

        // CONFIG
        void getConfigFile(std::string &) const;
        void getBTXParam(const char *, std::string &, std::string &) const;
        void getDbParam(const char *, std::string &, std::string &, std::string &, std::string &) const;

        // Format return
        void formatCandles(std::string &, std::vector<std::vector<double>> &) const;
        void formatReturnOrder(std::string &, std::unique_ptr<Order> &) const;
        void formatReturnWallet(std::string &, std::string const &, double &) const;
        void formatReturnStatus(std::string &, std::unique_ptr<Order> &) const;

        // Autre
        std::string getCurrentDate() const;
   private:
        void _clearOCHL(std::vector<std::vector<double>> &) const;
        void _reverseOCHL(std::vector<std::vector<double>> &) const;
        std::string _returnError = "error";
        std::string _returnSuccess = "SUCCESS";
};

#endif // UTIL_H
