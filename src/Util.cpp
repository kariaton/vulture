#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <string>
#include <algorithm> // reverse
#include <type_traits>
#include <ctime>

#include "Util.h"
#include "Bitfinex.h"
#include "Order.h"

using namespace rapidjson;
using namespace std;

Util::Util()
{
    //ctor
}

Util::~Util()
{
    //dtor
}

/*
* Return array
* document["hello"].getString() = "world"
* https://rapidjson.org/md_doc_tutorial.html#ValueDocument
*/
void Util::getBTXParam(const char *config, string &apiKey, string &apiKeySecret) const
{
    Document document;
    document.Parse(config);

    apiKey = document["btx-api-key"].GetString();
    apiKeySecret = document["btx-api-key-secret"].GetString();
}

void Util::getDbParam(const char *config, string &dbHost, string &dbName, string &dbUser, string &dbPassword) const
{
    Document document;
    document.Parse(config);

    dbHost = document["db-host"].GetString();
    dbName = document["db-name"].GetString();
    dbUser = document["db-user"].GetString();
    dbPassword = document["db-password"].GetString();
}

void Util::formatCandles(string &stringData, vector<vector<double>> &candles) const
{
    const char *json = stringData.c_str();
    _clearOCHL(candles);

    Document document;
    document.Parse(json);

    const Value &dataList = document;

    // if we ask all candles.
    if (dataList[0].IsArray()) {
        for(SizeType i = 0; i < dataList.Size(); ++i) {
            const Value &data = dataList[i];
            candles[Bitfinex::candleOCHL::OPEN].push_back(data[Bitfinex::candleOCHL::OPEN].GetDouble());
            candles[Bitfinex::candleOCHL::CLOSE].push_back(data[Bitfinex::candleOCHL::CLOSE].GetDouble());
            candles[Bitfinex::candleOCHL::HIGH].push_back(data[Bitfinex::candleOCHL::HIGH].GetDouble());
            candles[Bitfinex::candleOCHL::LOW].push_back(data[Bitfinex::candleOCHL::LOW].GetDouble());
        }

        _reverseOCHL(candles);

    } else { // Juste last candle
        candles[Bitfinex::candleOCHL::OPEN].push_back(dataList[Bitfinex::candleOCHL::OPEN].GetDouble());
        candles[Bitfinex::candleOCHL::CLOSE].push_back(dataList[Bitfinex::candleOCHL::CLOSE].GetDouble());
        candles[Bitfinex::candleOCHL::HIGH].push_back(dataList[Bitfinex::candleOCHL::HIGH].GetDouble());
        candles[Bitfinex::candleOCHL::LOW].push_back(dataList[Bitfinex::candleOCHL::LOW].GetDouble());
    }
}

void Util::formatReturnOrder(string &stringData, unique_ptr<Order> &order) const
{
    const char *json = stringData.c_str();
    Document document;
    document.Parse(json);
    const Value &dataList = document;

    if (dataList.Capacity() >= 1 && dataList[0].IsString() && dataList[0].GetString() == _returnError) { // Erreur
        order->setStatus(dataList[0].GetString());
        order->setCode(dataList[1].IsNull() ? 0 : dataList[1].GetInt());
        order->setMessage(dataList[2].GetString());
    } else if (dataList.Capacity() >= 6 && dataList[6].IsString() && dataList[6].GetString() == _returnSuccess) { // Order passé sur btx
        order->setStatus(dataList[6].GetString());
        order->setDate(getCurrentDate());
        const Value &datas = dataList[4];
        const Value &data = datas[0].IsArray() ? datas[0] : datas; // Submit retourne un [[]]
        order->setBtxId(to_string(data[0].GetInt64()));
    } else if (dataList.Capacity() >= 1 && dataList[0].IsArray()) {
        const Value &data = dataList[0];
        order->setStatus(data[13].GetString());
    }
}

void Util::formatReturnStatus(string &stringData, unique_ptr<Order> &order) const
{
    const char *json = stringData.c_str();
    Document document;
    document.Parse(json);
    const Value &dataList = document;

    if (!dataList.Empty() && dataList[0].IsArray()) {
        const Value &data = dataList[0];
        order->setStatus(data[13].GetString());
    } else {
        order->setStatus("");
    }
}

void Util::formatReturnWallet(string &stringData, string const &currency, double &balance) const
{
    cout << stringData << endl;
    const char *json = stringData.c_str();
    Document document;
    document.Parse(json);
    const Value &dataList = document;

    for (size_t i = 0; i < dataList.Size(); ++i) {
        const Value &data = dataList[i];

        if (data[1].GetString() == currency) {
            balance = data[2].GetDouble();
        }
    }
}

void Util::getConfigFile(string &config) const
{
    string line = "";
    ifstream file("config.json");

    if (!file) {
        cerr << "erreur init config.json" << endl;
    }

    while(getline(file, line)) {
        config += line;
    }
}

void Util::_clearOCHL(vector<vector<double>> &candles) const
{
    for(auto &candleOCHL : candles) {
        candleOCHL.clear();
    }
}

void Util::_reverseOCHL(vector<vector<double>> &candles) const
{
    for(auto &candleOCHL : candles) {
        reverse(candleOCHL.begin(), candleOCHL.end());
        candleOCHL.pop_back();
    }
}

string Util::getCurrentDate() const
{
    time_t rawtime;
    tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d %H-%M-%S", timeinfo);
    return buffer;
}
