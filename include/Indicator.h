#ifndef INDICATOR_H
#define INDICATOR_H

#include "ta_libc.h"


class Indicator
{
    public:
        Indicator();
        virtual ~Indicator();

        void macd(std::vector<double> const &) const;
        void stochRsi(std::vector<double> const &) const;
        void stochF(std::vector<double> const &, std::vector<double> const &, std::vector<double> const &, int &, int &, double *, double *) const;
        void stochRsiIsUp() const;

    protected:

    private:
        TA_RetCode _res;
        double *_outFastK;
        double *_outFastD;
};

#endif // INDICATOR_H
