#ifndef MYSQL_H
#define MYSQL_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "Util.h"
#include "Order.h"

class Mysql
{
    public:
        Mysql();
        virtual ~Mysql();

        void test() const;
        void newOrder(Order &order) const;

    private:
        Util _util = Util();

        sql::Driver *_driver;
        sql::Connection *_con;
        sql::Statement *_stmt;

        void _init();
};

#endif // MYSQL_H
