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
    int stochrsiSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_STOCHRSI_Lookback(14,14,14, TA_MAType_SMA);
    double *outMacd = new double[macdSize];
    double *outMacdSignal = new double[macdSize];
    double *outMacdHist = new double[macdSize];
    double *outFastK = new double[stochrsiSize];
    double *outFastD = new double[stochrsiSize];
    int outBegIdx= 0;
    int outNbElement = 0;

    // Bdd
    Mysql *mysql = new Mysql();

    for(;;) {
        try {
            bitfinex->candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**StochRSI**/
            indicator->stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
            //cout << "StochRSI outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;

            /**StochF**/
            indicator->stochF(
                candles[Bitfinex::candleOCHL::HIGH], candles[Bitfinex::candleOCHL::LOW], candles[Bitfinex::candleOCHL::CLOSE],
                outBegIdx, outNbElement, outFastK, outFastD
            );
            //cout << "StochF outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;

            bitfinex->candles(candles,Bitfinex::ASK_LAST_CANDLES);
            cout << "Last : " << candles[Bitfinex::candleOCHL::CLOSE][0] << endl;

        } catch (string const &e) {
            cerr << e << endl;
        }

        cout << "---------" << endl;
        usleep(2000000);
    }

    delete outMacd;
    outMacd = nullptr;
    delete outMacdSignal;
    outMacdSignal = nullptr;
    delete outMacdHist;
    outMacdHist = nullptr;
    delete outFastK;
    outFastK = nullptr;
    delete outFastD;
    outFastD = nullptr;
    delete bitfinex;
    bitfinex = nullptr;
    delete indicator;
    indicator = nullptr;
    delete mysql;
    mysql = nullptr;

    return 0;
}
