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

//    for(;;) {
//        try {
//            bitfinex->candles(candles, Bitfinex::ASK_ALL_CANDLES);
//
//            /**INDICATEUR**/
//            indicator->stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
//            indicator->stochF(candles);
//
//            bitfinex->candles(candles, Bitfinex::ASK_LAST_CANDLES);
//            //lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
//
//
//            // Achat.
//            if (indicator->stochRsiIsUp() && indicator->stochFIsUp() && !cycleOpen) {
//                cycleOpen = true;
//                // ON Achète
//            } else if(!indicator->stochRsiIsUp() || !indicator->stochFIsUp()) {
//
//            }
//        } catch (string const &e) {
//            cerr << e << endl;
//        }
//
//        cout << "---------" << endl;
//        usleep(2000000);
//    }

    return 0;
}
