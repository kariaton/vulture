#include <iostream>
#include <vector>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Mysql.h"

using namespace std;

int main()
{
    Indicator *indicator = new Indicator();
    Bitfinex *bitfinex = new Bitfinex();

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    // Bdd
    Mysql *mysql = new Mysql();

    for(;;) {
        try {
            bitfinex->candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**INDICATEUR**/
            indicator->stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
            cout << indicator->stochRsiIsUp() << endl;
            indicator->stochF(candles);
            cout << indicator->stochFIsUp() << endl;

            bitfinex->candles(candles, Bitfinex::ASK_LAST_CANDLES);
            cout << "Last : " << candles[Bitfinex::candleOCHL::CLOSE][0] << endl;

        } catch (string const &e) {
            cerr << e << endl;
        }

        cout << "---------" << endl;
        usleep(2000000);
    }

    bitfinex = nullptr;
    delete indicator;
    indicator = nullptr;
    delete mysql;
    mysql = nullptr;

    return 0;
}
