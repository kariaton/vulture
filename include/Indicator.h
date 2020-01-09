#ifndef INDICATOR_H
#define INDICATOR_H

#include "ta_libc.h"


class Indicator
{
    public:
        Indicator();
        virtual ~Indicator();

        void stochRsi(std::vector<double> const &);
        void stochF(std::vector<std::vector <double>> const &);
        void stochRsiIsUp() const;
        bool stochRsiIsUp() {return _stochRsiIsUp;}
        bool stochFIsUp() {return _stochFIsUp;}

    private:
        TA_RetCode _res;
        double *_outFastK;
        double *_outFastD;
        bool _stochRsiIsUp;
        bool _stochFIsUp;
};

#endif // INDICATOR_H
