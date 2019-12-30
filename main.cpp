#include <iostream>
#include <vector>
#include <unistd.h>
#include <algorithm> // reverse

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"

using namespace std;

int main()
{
    Indicator *indicator = new Indicator();
    Bitfinex *bitfinex = new Bitfinex();

    // Generate datas
    vector <double> candels = {0};

    unsigned int microseconds = 50000;
    int dataSize = Bitfinex::CANDLES - TA_MACD_Lookback(12, 26, 9);
    int test = Bitfinex::CANDLES - TA_STOCHRSI_Lookback(14,14,14, TA_MAType_SMA);
    double *outMacd = new double[dataSize];
    double *outMacdSignal = new double[dataSize];
    double *outMacdHist = new double[dataSize];
    double *outFastK = new double[test];
    double *outFastD = new double[test];
    int outBegIdx= 0;
    int outNbElement = 0;

    for(;;) {
        try {
            bitfinex->candles(candels);
            reverse(candels.begin(), candels.end());
            /**MACD**/
            indicator->macd(candels, outBegIdx, outNbElement, outMacd, outMacdSignal, outMacdHist);
            cout <<" hist : " << outMacdHist[outNbElement-2] << " macd : " << outMacd[outNbElement-2] << " signal : " << outMacdSignal[outNbElement-2] << endl;

            /**StochRSI**/
            indicator->stochRsi(candels, outBegIdx, outNbElement, outFastK, outFastD);
            cout << "outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;
        } catch (string const &e) {
            cerr << e << endl;
        }

        cout << "---------" << endl;
        usleep(2000000);
    }

    delete outMacd;
    delete outMacdSignal;
    delete outMacdHist;
    delete outFastK;
    delete outFastD;

    delete bitfinex;
    bitfinex = nullptr;

    delete indicator;
    indicator = nullptr;

    return 0;
}
