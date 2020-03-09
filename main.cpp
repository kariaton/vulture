#include <iostream>
#include <vector>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Order.h"
#include "Mysql.h"
#include "Wallet.h"

using namespace std;

int main()
{
    // Trading
    Indicator indicator;
    Bitfinex bitfinex;
    unique_ptr<Order> order;
    double lastBtxClose; // Dernière trad sur la platforme
    bool cycleIsOpen = true;
    bool bbandBottomTouched = false;

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    // Bdd
    //Mysql mysql;

    // Wallet
    //Wallet wallet;
    //mysql.getWallet(wallet);

    for(;;) {
        try {
            bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);

//            /**INDICATEUR**/
            indicator.macd(candles[Bitfinex::candleOCHL::CLOSE]);
            indicator.stochF(candles);
            indicator.bband(candles[Bitfinex::candleOCHL::CLOSE]);

//            bitfinex.wallets();
//
            // On attand que le close touche la bband du bas
            if (indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && !order && !bbandBottomTouched) {
                cout << "touché" << endl;
                bbandBottomTouched = true;
            }

            // Si la bband à été touchée mais est remontée au dessus de la bb il faut acheter.
            if (bbandBottomTouched && !order && !indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE])) {

                bbandBottomTouched = false;
                unique_ptr<Order> newOrder(new Order());
                double walletUsd = 0;
                bitfinex.wallets("USD", walletUsd);
                int i = 0;
                do {
                    bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                    double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
                    double price = lastClose + 0.1;

                    newOrder->setBtxPrice(lastClose);
                    newOrder->setPrice(price);
                    newOrder->setAmount((walletUsd * 0.2) / price); // on achéte pour 20% du wallet usd

                    if (newOrder->getBtxId() == "") {
                        bitfinex.submit(newOrder);
                    } else {
                        // Update lastClose +0.1
                        bitfinex.update(newOrder);
                    }

                    usleep(1000000);
                    i++;
               } while(newOrder->getStatus() != "EXECUTED" && i < 4);

                // si au bout de 4 tentatives l'order n'a pas été acheté, on cancel
                if (newOrder->getStatus() != "EXECUTED") {
                    bitfinex.cancel(newOrder);
                    newOrder->setStatus("CANCEL");
                } else {
                    order = move(newOrder);
                }
            }

            // Si la bband du haut est touchée et qu'il y'a un order on vend
            if (indicator.bbandUpperIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && order) {
                bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
                double minPrice = order->getPrice() + (order->getPrice() * 0.10); // le price + 10%

                if (lastClose >= minPrice) {
                    unique_ptr<Order> newOrder(new Order());

                    int i = 0;
                    do {
                        bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                        double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];

                        newOrder->setBtxPrice(lastClose);
                        newOrder->setPrice(lastClose - 0.1); // Vente lastClose - 0.1
                        newOrder->setAmount(order->getAmount() * -1); // Montant négatif pour un ordre de vente

                        if (order->getBtxId() == "") {
                            bitfinex.submit(order);
                        } else {
                            // Update lastClose  - 0.1
                            bitfinex.update(order);
                        }
                        usleep(1000000);
                        i++;
                    } while(order->getStatus() != "EXECUTED" && i < 4);

                    // L'ordre n'est pas passé on le supprime sur btx
                    if (newOrder->getStatus() != "EXECUTED") {
                        bitfinex.cancel(newOrder);
                        newOrder->setStatus("CANCEL");
                    } else {
                        order.reset();
                    }
                }
            }
        } catch (string const &e) {
            cerr << e << endl;
        }

        cout << "---------" << endl;
        usleep(2000000);
    }

    return 0;
}
