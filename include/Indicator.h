#ifndef INDICATOR_H
#define INDICATOR_H

#include "ta_libc.h"


class Indicator
{
    public:
        Indicator();
        virtual ~Indicator();

        void macd(std::vector<double> &, int &, int &, double *, double *, double *) const;
        void stochRsi(std::vector<double> &, int &, int &, double *, double *) const;
        void stochF(double const *inHight, double const  *inLow, double const  *inClose, int &outBegIdx, int &outNBElement, double *outFastK, double *) const;

    protected:

    private:
        TA_RetCode _res;

};

#endif // INDICATOR_H
