#include <iostream>
#include <vector>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Order.h"

using namespace std;

int main()
{
    cout << "INIT VULTURE" << endl;
    cout << "v1.0.0" << endl;

    // Trading
    Indicator indicator;
    Bitfinex bitfinex;
    unique_ptr<Order> order;
    bool bbandBottomTouched = false;

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    for(;;) {
        try {
            bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**INDICATEUR**/
            indicator.bband(candles[Bitfinex::candleOCHL::CLOSE]);

            // On attand que le close touche la bband du
            if (indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && !order && !bbandBottomTouched) {
                bbandBottomTouched = true;
            }

            // Si la bband a été touchée mais est remontée au dessus de la bb il faut acheter.
            if (bbandBottomTouched && !order && !indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE])) {
                bbandBottomTouched = false;
                bitfinex.buy(order, candles);
            }

            // Si la bband du haut est touchée et qu'il y'a un order on vend
            if (indicator.bbandUpperIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && order) {
                bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];

                if (lastClose > order->getPrice()) {
                    bitfinex.sale(order, candles);
                }
            }
        } catch (string const &e) {
            cerr << e << endl;
        }

        usleep(2000000);
    }

    return 0;
}
