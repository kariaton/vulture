#include <iostream>
#include <vector>
#include <unistd.h>

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
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    int macdSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_MACD_Lookback(12, 26, 9);
    int stochrsiSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_STOCHRSI_Lookback(14,14,14, TA_MAType_SMA);
    double *outMacd = new double[macdSize];
    double *outMacdSignal = new double[macdSize];
    double *outMacdHist = new double[macdSize];
    double *outFastK = new double[stochrsiSize];
    double *outFastD = new double[stochrsiSize];
    int outBegIdx= 0;
    int outNbElement = 0;

    for(;;) {
        try {
            bitfinex->candles(candles);

            /**MACD**/
            indicator->macd(candles[Bitfinex::candleOCHL::CLOSE], outBegIdx, outNbElement, outMacd, outMacdSignal, outMacdHist);
            cout <<"MACD hist : " << outMacdHist[outNbElement-2] << " macd : " << outMacd[outNbElement-2] << " signal : " << outMacdSignal[outNbElement-2] << endl;

            /**StochRSI**/
            indicator->stochRsi(candles[Bitfinex::candleOCHL::CLOSE], outBegIdx, outNbElement, outFastK, outFastD);
            cout << "StochRSI outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;

            /**StochF**/
            indicator->stochF(
                candles[Bitfinex::candleOCHL::HIGH], candles[Bitfinex::candleOCHL::LOW], candles[Bitfinex::candleOCHL::CLOSE],
                outBegIdx, outNbElement, outFastK, outFastD
            );
            cout << "StochF outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;

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

    return 0;
}
