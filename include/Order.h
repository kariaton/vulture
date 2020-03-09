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

        std::string getDate() {return _date;};
        void setDate(std::string date) {_date = date;};

        std::string getType() {return _type;};
        void setType(std::string type) {_type = type;};

        double getPrice() {return _price;};
        void setPrice(double price) {_price = price;};

        double getSalePrice() {return _salePrice;};
        void setSalePrice(double salePrice) {_salePrice = salePrice;};

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

        std::string getIdPurchase() {return _idPurchase;};
        void setIdPurchase(std::string idPurchase) {_idPurchase = idPurchase;};

    protected:

    private:
        std::string _btxId;
        std::string _date;
        std::string _type;
        double _price;
        double _salePrice;
        double _btxPrice;
        double _amount;
        int _code;
        std::string _status;
        std::string _message;
        std::string _idPurchase;
};

#endif // ORDER_H
