#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <string>

class Order
{
    public:
        Order();
        virtual ~Order();

        std::string getBtxId() {return _btxId;};
        void setBtxId(std::string btxId) {_btxId = btxId;};
        double getPrice() {return _price;};
        void setPrice(double price) {_price = price;};
        double getBtxPrice() {return _btxPrice;};
        void setBtxPrice(double btxPrice) {_btxPrice = btxPrice;};
        double getAmount() {return _amount;};
        void setAmount(double amount) {_amount = amount;};
        int getCode() {return _code;};
        void setCode(int code) {_code = code;};
        std::string getStatus() {return _status;};
        void setStatus(std::string status) {_status = status;};
        std::string getMessage() {return _message;};
        void setMessage(std::string message) {_message = message;};

    protected:

    private:
        std::string _btxId;
        double _price;
        double _btxPrice;
        double _amount;
        int _code;
        std::string _status;
        std::string _message;
};

#endif // ORDER_H
