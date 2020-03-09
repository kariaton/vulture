#ifndef INDICATOR_H
#define INDICATOR_H

#include "ta_libc.h"

class Indicator
{
    public:
        Indicator();
        virtual ~Indicator();

        void bband(std::vector<double> const &);
        void macd(std::vector<double> const &);
        void stochRsi(std::vector<double>  &);
        void stochF(std::vector<std::vector <double>> const &);
        bool bbandBottomIsTouched(std::vector<double> const &);
        bool bbandUpperIsTouched(std::vector<double> const &);
        bool stochRsiIsUp() {return _stochRsiIsUp;}
        bool stochFIsUp() {return _stochFIsUp;}

    private:
        TA_RetCode _res;
        bool _stochRsiIsUp;
        bool _stochFIsUp;
        double _bbandBottom;
        double _bbandUpper;
};

#endif // INDICATOR_H
