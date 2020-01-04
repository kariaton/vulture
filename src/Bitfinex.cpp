#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/hmac.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

#include <iostream>
#include <curl/curl.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <map>

#include "Bitfinex.h"
#include "Util.h"

using namespace std;
using namespace CryptoPP;


Bitfinex::Bitfinex()
{
    _init();
    _util = Util();
}

Bitfinex::~Bitfinex()
{
  //dtor
}

void Bitfinex::_init()
{
    string config = "";
    string apiKey = "";
    string apiKeySecret = "";

    _util.getConfigFile(config);
    _util.getBTXParam(config.c_str(), apiKey, apiKeySecret);

    _apiKey = apiKey;
    _apiKeySecret = apiKeySecret;

}

void Bitfinex::candles(vector<vector<double>> &candles) const
{
    string endpoint = "candles";
    string sNbCandles = std::to_string(Bitfinex::CANDLES_NUMBER_ELEMENT );
    string param = "trade:15m:tBTCUSD/hist?limit=" + sNbCandles;
    string response = "";

    get(endpoint, param, response);

    _util.formatCandles(response, candles);
}

void Bitfinex::wallets(string &response) const
{
    string endpoint = "r/wallets";
    string body = "{}";
    post(endpoint, body, response);
}

int Bitfinex::get(string const &enpoint, string const &param, string &response) const
{
    string url = _publicUrl + "/" + enpoint + "/" + param;

    CURL *curl;
    CURLcode resCode;
    curl = curl_easy_init();

    struct curl_slist *headers = NULL;
    curl_slist_append(headers, "cache-control: no-cache");
    curl_slist_append(headers, "Connection: keep-alive");
    curl_slist_append(headers, "Accept-Encoding: gzip, deflate");
    curl_slist_append(headers, "Host: api-pub.bitfinex.com");
    curl_slist_append(headers, "Cache-Control: no-cache");
    curl_slist_append(headers, "Accept: */*");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, CURL_DEBUG_VERBOSE);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Bitfinex::writeCallback);

    resCode = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (resCode != CURLE_OK) {
        throw string("ERREUR : libcurl error in Request.get(): CURLCode : " + to_string(resCode));
    }

    return 0;
}

int Bitfinex::post(string  const &endpoint, string const &body, string &response) const
{
    string nonce = "", signature = "", sig = "";
    string url = _privateUrl + "/" + _entrypoint + "/" + endpoint;

    getNonce(nonce);
    getSignature(nonce, body, endpoint, signature);
    getSig(signature, sig);

    struct curl_slist *curlHeader = nullptr;
    curlHeader = curl_slist_append(curlHeader, ("bfx-nonce:" + nonce).c_str());
    curlHeader = curl_slist_append(curlHeader, ("bfx-apikey:"+ _apiKey).c_str());
    curlHeader = curl_slist_append(curlHeader, ("bfx-signature:" + sig).c_str());
    curlHeader = curl_slist_append(curlHeader, "Content-Type: application/json");

    CURL *curl;
    CURLcode resCode;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeader);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, CURL_DEBUG_VERBOSE);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Bitfinex::writeCallback);

        /* Perform the request, res will get the return code */
        resCode = curl_easy_perform(curl);
        /* Check for errors */
        if(resCode != CURLE_OK) {
            cerr << "libcurl error in request post:" << endl;
            cerr << "CURL CODE : " << resCode << endl;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    } else {
        cerr << "Error curl init" << endl;
    }

  return 0;
}

void Bitfinex::getSignature(string const &nonce, string const &body, string const &endpoint, string &signature) const
{
    signature = "/api/"+_entrypoint+"/"+endpoint+nonce+body;
}

void Bitfinex::getSig(string &signature, string &sig) const
{
    SecByteBlock byteKey((const byte*)_apiKeySecret.data(), _apiKeySecret.size());
    string mac;
    sig.clear();

    HMAC <SHA384> hmac(byteKey, byteKey.size());
    StringSource ss1(
        signature,
        true,
        new HashFilter(hmac, new StringSink(mac))
    );
    StringSource ss2(
        mac,
        true,
        new HexEncoder(new StringSink(sig))
    );

    transform(sig.cbegin(), sig.cend(), sig.begin(), ::tolower);
}

void Bitfinex::getNonce(string &nonce) const
{
    time_t iNonce = time(nullptr) *1000;

    // transform int to string
    stringstream ss;
    ss << iNonce;

    nonce = ss.str();
}

void Bitfinex::getBase64(string &body, string &payload) const
{
    byte buffer[1024] = {};

    for (size_t i = 0; i < body.length(); ++i) {
        buffer[i] = body[i];
    }

    StringSource ss(
        buffer,
        body.length(),
        true,
        new Base64Encoder(new StringSink(payload), false)
    );
}

size_t Bitfinex::writeCallback(void *data, size_t size, size_t nmemb, void *userp) noexcept
{
    (static_cast <string*>(userp))->append(static_cast <char*>(data), size * nmemb);

    return size * nmemb;
};


