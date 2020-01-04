#include <iostream>
#include <vector>
#include <ta_libc.h>

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
    _res = TA_Shutdown();
}

void Indicator::macd(vector<double> const &close, int &outBegIdx, int &outNbElement, double *outMacd, double *outMacdSignal, double *outMacdHist) const
{
    double const *aClose = &close[0];

    TA_MACD(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        12, 26, 9,
        &outBegIdx, &outNbElement,
        outMacd, outMacdSignal, outMacdHist
    );
}

void Indicator::stochRsi(vector<double> const &close, int &outBegIdx, int &outNbElement, double *outFastK, double *outFastD) const
{
    double const *aClose = &close[0];

    TA_STOCHRSI(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        14, 14, 14, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        outFastK, outFastD
    );
}

void Indicator::stochF(vector<double> const &inHigh, vector<double> const &inLow, vector<double> const &inClose, int &outBegIdx, int &outNBElement, double *outFastK, double *outFastD) const
{
    double const *aInHigh = &inHigh[0];
    double const *aInLow = &inLow[0];
    double const *aInClose = &inClose[0];

    TA_STOCHF(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aInHigh, aInLow, aInClose, // double[]
        14, 3, TA_MAType_SMA,
        &outBegIdx, &outNBElement,
        outFastK, outFastD // double[]
    );
}
