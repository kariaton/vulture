#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <string>
#include <algorithm> // reverse

#include "Util.h"
#include "Bitfinex.h"

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

    for(SizeType i = 0; i < dataList.Size(); ++i) {
        const Value &data = dataList[i];
        candles[Bitfinex::candleOCHL::OPEN].push_back(data[Bitfinex::candleOCHL::OPEN].GetDouble());
        candles[Bitfinex::candleOCHL::CLOSE].push_back(data[Bitfinex::candleOCHL::CLOSE].GetDouble());
        candles[Bitfinex::candleOCHL::HIGH].push_back(data[Bitfinex::candleOCHL::HIGH].GetDouble());
        candles[Bitfinex::candleOCHL::LOW].push_back(data[Bitfinex::candleOCHL::LOW].GetDouble());
    }

    _reverseOCHL(candles);
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
    }
}
