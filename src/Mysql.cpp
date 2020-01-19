#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "Mysql.h"
#include "Util.h"
#include "Order.h"

using namespace std;
using namespace sql;

Mysql::Mysql()
{
    _init();
}

Mysql::~Mysql()
{
    delete _con;
    delete _stmt;
}

void Mysql::_init()
{
    string config = "";
    string dbHost = "";
    string dbName = "";
    string dbUser = "";
    string dbPassword = "";

    _util.getConfigFile(config);
    _util.getDbParam(config.c_str(), dbHost, dbName, dbUser, dbPassword);

    _driver = get_driver_instance();
    _con = _driver->connect(dbHost, dbUser, dbPassword);
    _con->setSchema(dbName);
    _stmt = _con->createStatement();
}

void Mysql::test() const
{
    ResultSet* res;
    res = _stmt->executeQuery("SELECT * FROM test");
    try {
        while (res->next()) {
            cout << res->getString("nom") << endl;
        }
    } catch (SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (code erreur MySQL: " << e.getErrorCode();
        cout << ", EtatSQL: " << e.getSQLState() << " )" << endl;
    }

    delete res;
}

void Mysql::newOrder(Order &order) const
{
    string btxId = order.getBtxId();
    string amount = to_string(order.getAmount());
    string price = to_string(order.getPrice());
    string status = order.getStatus();
    string date = order.getDate();
    string btxPrice = to_string(order.getBtxPrice());

    try {
        string req = "INSERT INTO orders (btx_id, amount, price, btx_price, type, status, date) VALUES ('"+btxId+"', "+amount+", "+price+", "+btxPrice+", 'created', '"+status+"', '"+date+"')";
        _stmt->executeUpdate(req);
    } catch (SQLException &e) {
        cerr << "# ERR: " << e.what();
        cerr << " (code erreur MySQL: " << e.getErrorCode();
        cerr << ", EtatSQL: " << e.getSQLState() << " )" << endl;
    }
}
