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

void Indicator::macd(vector<double> &data, int &outBegIdx, int &outNbElement, double *outMacd, double *outMacdSignal, double *outMacdHist) const
{
    double *aData = &data[0];

    TA_MACD(
        0, Bitfinex::CANDLES-1,
        aData,
        12, 26, 9,
        &outBegIdx, &outNbElement,
        outMacd, outMacdSignal, outMacdHist
    );
}

void Indicator::stochRsi(vector<double> &data, int &outBegIdx, int &outNbElement, double *outFastK, double *outFastD) const
{
    double *aData = &data[0];

    TA_STOCHRSI(
        0, Bitfinex::CANDLES-1,
        aData,
        14, 14, 14, TA_MAType_SMA,
        &outBegIdx, &outNbElement,
        outFastK, outFastD
    );
}

void Indicator::stochF(double const *inHight, double const  *inLow, double const  *inClose, int &outBegIdx, int &outNBElement, double *outFastK, double *outFastD) const
{
    TA_STOCHF(
        0, Bitfinex::CANDLES-1,
        inHight, inLow, inClose, // double[]
        14, 3, TA_MAType_SMA,
        &outBegIdx, &outNBElement,
        outFastK, outFastD // double[]
    );
}
