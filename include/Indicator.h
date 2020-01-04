#ifndef INDICATOR_H
#define INDICATOR_H

#include "ta_libc.h"


class Indicator
{
    public:
        Indicator();
        virtual ~Indicator();

        void macd(std::vector<double> const &, int &, int &, double *, double *, double *) const;
        void stochRsi(std::vector<double> const &, int &, int &, double *, double *) const;
        void stochF(std::vector<double> const &, std::vector<double> const &, std::vector<double> const &, int &, int &, double *, double *) const;

    protected:

    private:
        TA_RetCode _res;
};

#endif // INDICATOR_H
