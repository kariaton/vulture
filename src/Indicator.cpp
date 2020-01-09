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

    int stochrsiSize = Bitfinex::CANDLES_NUMBER_ELEMENT - TA_STOCHRSI_Lookback(14,14,14, TA_MAType_SMA);
    _outFastK = new double[stochrsiSize];
    _outFastD = new double[stochrsiSize];
}

Indicator::~Indicator()
{
    _res = TA_Shutdown();
    delete _outFastK;
    _outFastK = nullptr;
    delete _outFastD;
    _outFastD = nullptr;
}

void Indicator::macd(vector<double> const &close) const
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

void Indicator::stochRsi(vector<double> const &close) const
{
    double const *aClose = &close[0];
    int outNbElement = 0;
    int outBegIdx = 0;

    TA_STOCHRSI(
        0, Bitfinex::CANDLES_NUMBER_ELEMENT-1,
        aClose,
        14, 14, 14, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        _outFastK, _outFastD
    );

    cout << "StochRSI outFastK : " << _outFastK[outNbElement-2] << " outFasD : " << _outFastD[outNbElement-2] << endl;
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
