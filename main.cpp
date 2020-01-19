#include <iostream>
#include <vector>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Order.h"
#include "Mysql.h"

using namespace std;

int main()
{
    // Trading
    Indicator indicator;
    Bitfinex bitfinex;
    Order order;
    double lastBtxClose; // Dernière trad sur la platforme
    bool cycleOpen = false;

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};
    // Bdd
    Mysql mysql;

    for(;;) {
        try {
            bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**INDICATEUR**/
            indicator.stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
            indicator.stochF(candles);

            /** ACHAT **/
            if (!cycleOpen) {
            //if (indicator.stochRsiIsUp() && indicator.stochFIsUp() && !cycleOpen && order.getBtxId() == "") {
                cycleOpen = true;

                int i = 0;
                do {
                    bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                    double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
                    lastClose = 1.1; // delete

                    order.setBtxPrice(lastClose);
                    order.setPrice(lastClose + 0.1); // Achat lastClose +0.1
                    // TODO gestion du amount
                    order.setAmount(1.1);

                    if (order.getBtxId() == "") {
                        bitfinex.submit(order);
                    } else {
                        // Update lastClose +0.1
                        bitfinex.update(order);
                    }
                    usleep(1000000);
                    i++;
                } while(order.getStatus() != "EXECUTED" && i < 4);

                // si au bout de 4 tentatives l'order n'a pas été acheté, on cancel
                if (order.getStatus() != "EXECUTED") {
                    bitfinex.cancel(order);
                    order.setStatus("CANCEL");
                }

                // Enregistrement bdd;
                mysql.newOrder(order);

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
