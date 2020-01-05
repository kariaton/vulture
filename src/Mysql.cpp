#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "Mysql.h"
#include "Util.h"

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
