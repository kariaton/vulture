#include <iostream>
#include <vector>
#include <ta_libc.h>
#include <algorithm> // reverse

#include "Indicator.h"
#include "Bitfinex.h"

using namespace std;

Indicator::Indicator()
{
    _res = TA_Initialize();
    if (_res != TA_SUCCESS) {
        cerr << "Erreur TA_Lib initialisation" << endl;
    }
}

Indicator::~Indicator()
{
    _res = TA_Shutdown();;
}
/**
* indicator.stochRsi(candles[Bitfinex::candleOCHL::CLOSE]);
*/
void Indicator::stochRsi(vector<double>  &close)
{
    double const *aClose = &close[0];
    int outNbElement = 0;
    int outBegIdx = 0;
    int stochRsiSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_STOCHRSI_Lookback(14, 14, 14, TA_MAType_SMA);
    double *outFastK = new double[stochRsiSize];
    double *outFastD = new double[stochRsiSize];


    TA_STOCHRSI(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        14, 14, 14, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        outFastK, outFastD
    );

    //cout << "StochRSI outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;
    _stochRsiIsUp = outFastK > outFastD;

    delete outFastK;
    delete outFastD;
}

/**
* indicator.macd(candles[Bitfinex::candleOCHL::CLOSE]);
*/
void Indicator::macd(vector<double> const &close)
{
    double const *aClose = &close[0];
    int outNbElement = 0;
    int outBegIdx = 0;
    int macdSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_MACD_Lookback(12,26,9);

    double *outMacd = new double[macdSize];
    double *outMacdSignal = new double[macdSize];
    double *outMacdHist = new double[macdSize];

    _res = TA_MACD(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        10, 26, 9,
        &outBegIdx, &outNbElement,
        outMacd, outMacdSignal, outMacdHist
    );

    //cout << "macd : " << outMacd[outNbElement-2] << " signal : " << outMacdSignal[outNbElement-2] << " hist : " << outMacdHist[outNbElement-2] << endl;

    delete outMacd;
    delete outMacdSignal;
    delete outMacdHist;
}

/**
* indicator.stochF(candles);
*/
void Indicator::stochF(vector<vector <double>> const &candles)
{
    double const *aInHigh = &candles[Bitfinex::candleOCHL::HIGH][0];
    double const *aInLow = &candles[Bitfinex::candleOCHL::LOW][0];
    double const *aInClose = &candles[Bitfinex::candleOCHL::CLOSE][0];
    int stochfSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_STOCHF_Lookback(14, 3, TA_MAType_SMA);
    int outNbElement = 0;
    int outBegIdx = 0;
    double *outFastK = new double[stochfSize];
    double *outFastD = new double[stochfSize];

    _res = TA_STOCHF(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aInHigh, aInLow, aInClose, // double[]
        14, 3, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        outFastK, outFastD // double[]
    );

    //cout << "StochF outFastK : " << outFastK[outNbElement-2] << " outFasD : " << outFastD[outNbElement-2] << endl;

    _stochFIsUp = outFastK > outFastD;

    delete outFastK;
    delete outFastD;
}

/**
* indicator.bband(candles[Bitfinex::candleOCHL::CLOSE]);
*/
void Indicator::bband(vector<double> const &close)
{
    double const *aClose = &close[0];
    int outNbElement = 0;
    int outBegIdx = 0;
    int bbandSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_BBANDS_Lookback(20, 2, 2,TA_MAType_SMA);
    double *upper = new double[bbandSize];
    double *lower = new double[bbandSize];
    double *sma = new double[bbandSize];

    _res = TA_BBANDS(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        20, 2, 2, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        upper, sma, lower
    );

    //cout << "sma : " << sma[outNbElement-2] << " upper : " << upper[outNbElement-2] << " lower : " << lower[outNbElement-2] << endl;

    _bbandBottom = lower[outNbElement-2];
    _bbandUpper = upper[outNbElement-2];

    delete upper;
    delete lower;
    delete sma;
}

/**
* Check si le close touche la bband du bas.
*/
bool Indicator::bbandBottomIsTouched(vector<double> const &close)
{
    return close.back() <= _bbandBottom;
}

/**
* Ceck si le close touche la bband du haut.
*/
bool Indicator::bbandUpperIsTouched(vector<double> const &close)
{
    return close.back() >= _bbandUpper;
}

