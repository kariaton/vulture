#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <string>

#include "Util.h"

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
void Util::getBTXParam(const char* config, string &apiKey, string &apiKeySecret) const
{
    Document document;
    document.Parse(config);

    apiKey = document["btx-api-key"].GetString();
    apiKeySecret = document["btx-api-key-secret"].GetString();
}

void Util::macd(string &stringData, vector<double> &candles) const
{
    const char *json = stringData.c_str();
    candles.clear();

    Document document;
    document.Parse(json);

    const Value &dataList = document;

    for(SizeType i = 0; i < dataList.Size(); ++i) {
        const Value &data = dataList[i];
        candles.push_back(data[2].GetDouble());
    }

    candles.erase(candles.begin());
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
