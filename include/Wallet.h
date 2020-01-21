#ifndef WALLET_H
#define WALLET_H


class Wallet
{
    public:
        Wallet();
        virtual ~Wallet();

        double getHoldCoin() {return _holdCoin;};
        void setHoldCoin(double const holdCoin) {_holdCoin = holdCoin;};

        double getHoldUsd() {return _holdUsd;};
        void setHoldUsd(double const holdUsd) {_holdUsd = holdUsd;};

        double getAvailableUsd() {return _availableUsd;};
        void setAvailableUsd(double const availableUsd) {_availableUsd = availableUsd;};
    protected:

    private:
        double _holdCoin;
        double _holdUsd;
        double _availableUsd;
};

#endif // WALLET_H
