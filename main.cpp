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
    unique_ptr<Order> currentOrder;
    double lastBtxClose; // Dernière trad sur la platforme
    bool cycleOpen = false;

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    // Bdd
    Mysql mysql;

    // Wallet
    Wallet wallet;
    mysql.getWallet(wallet);

    for(;;) {
        try {
            bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**INDICATEUR**/
            indicator.stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
            indicator.stochF(candles);

            /** ACHAT **/
            if (indicator.stochRsiIsUp() && indicator.stochFIsUp() && !cycleOpen && !currentOrder) {
                cycleOpen = true;
                unique_ptr<Order> order(new Order());
                currentOrder = move(order);

                int i = 0;
                do {
                    bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                    double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];

                    currentOrder->setBtxPrice(lastClose);
                    currentOrder->setPrice(lastClose + 0.1); // Achat lastClose +0.1
                    currentOrder->setAmount(wallet.getAvailableUsd() / currentOrder->getPrice());

                    if (currentOrder->getBtxId() == "") {
                        bitfinex.submit(currentOrder);
                    } else {
                        // Update lastClose +0.1
                        bitfinex.update(currentOrder);
                    }
                    usleep(1000000);
                    i++;
                } while(currentOrder->getStatus() != "EXECUTED" && i < 4);

                // si au bout de 4 tentatives l'order n'a pas été acheté, on cancel
                if (currentOrder->getStatus() != "EXECUTED") {
                    bitfinex.cancel(currentOrder);
                    currentOrder->setStatus("CANCEL");
                }

                // Enregistrement bdd;
                mysql.newOrder(currentOrder);
                order = move(currentOrder);

            /** VENTE **/
            } else if(!indicator.stochRsiIsUp() || !indicator.stochFIsUp()) {

            }
        } catch (string const &e) {
            cerr << e << endl;
        }

        cout << "---------" << endl;
        usleep(2000000);
    }

    return 0;
}
