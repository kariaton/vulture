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
#include <memory>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "Order.h"

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

    _privateUrl = "https://api.bitfinex.com";
    _publicUrl = "https://api-pub.bitfinex.com/";
    _entrypoint = "auth";
    _version = "v2";

}

void Bitfinex::candles(vector<vector<double>> &candles, const bool &last) const
{
    string endpoint = "candles";
    string sNbCandles = to_string(Bitfinex::CANDLES_NUMBER_ELEMENT );
    string response = "";
    string param;

    if (last) {
         param = "trade:1m:tBTCUSD/last?limit=" + sNbCandles;
    } else {
        param = "trade:1m:tBTCUSD/hist?limit=" + sNbCandles;
    }

    get(endpoint, param, response);
    _util.formatCandles(response, candles);
}

void Bitfinex::status(unique_ptr<Order> &order) const
{
    string endpoint = "r/orders";
    string body = "{\"id\":["+order->getBtxId()+"]}";
    string response = "";
    post(endpoint, body, response);

    _util.formatReturnStatus(response, order);

    if (order->getStatus() == "") {
        endpoint = "r/orders/tBTCUSD/hist";
        body = "{\"id\":["+order->getBtxId()+"]}";
        response = "";
        post(endpoint, body, response);

        _util.formatReturnStatus(response, order);

        if (order->getStatus() != "") {
            order->setStatus("EXECUTED");
        }

    }
}

/**
* Envoie un order d'achat ou de vente
*/
void Bitfinex::submit(unique_ptr<Order> &order) const
{
    string endpoint = "w/order/submit";
    string body = "{\"type\":\"EXCHANGE LIMIT\",\"symbol\":\"tBTCUSD\",\"price\":\""+to_string(order->getPrice())+"\",\"amount\":\""+to_string(order->getAmount())+"\"}";
    string response = "";
    post(endpoint, body, response);

    _util.formatReturnOrder(response, order);
}

/**
* Update un order
*/
void Bitfinex::update(unique_ptr<Order> &order) const
{
    string endpoint = "w/order/update";
    string body = "{\"id\":"+order->getBtxId()+",\"price\":\""+to_string(order->getPrice())+"\",\"amount\":\""+to_string(order->getAmount())+"\"}";
    string response = "";
    post(endpoint, body, response);

    _util.formatReturnOrder(response, order);
}

/**
* Cancel un order
*/
void Bitfinex::cancel(unique_ptr<Order> &order) const
{
    string endpoint = "w/order/cancel";
    string body = "{\"id\":"+order->getBtxId()+"}";
    string response = "";
    post(endpoint, body, response);

    _util.formatReturnOrder(response, order);
}

void Bitfinex::order(unique_ptr<Order> &order) const
{
    string endpoint = "r/orders";
    string body = "{\"id\":["+order->getBtxId()+"]}";
    string response = "";
    post(endpoint, body, response);

    _util.formatReturnOrder(response, order);
}

void Bitfinex::wallets(string const &currency, double &balance) const
{
    string endpoint = "r/wallets";
    string body = "{}";
    string response = "";

    post(endpoint, body, response);

    _util.formatReturnWallet(response, currency, balance);
}

int Bitfinex::get(string const &enpoint, string const &param, string &response) const
{
    string url = _publicUrl + "/" + _version + "/" +  enpoint + "/" + param;

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
    string url = _privateUrl + "/"+ _version + "/" + _entrypoint + "/" + endpoint;

    this->getNonce(nonce);
    this->getSignature(nonce, body, endpoint, signature);
    this->getSig(signature, sig);

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

    //cout << "Réponse : " << response << endl;

  return 0;
}

void Bitfinex::getSignature(string const &nonce, string const &body, string const &endpoint, string &signature) const
{
    signature = "/api/" + _version + "/" + _entrypoint + "/" + endpoint+nonce+body;
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

void Bitfinex::buy(unique_ptr<Order> &order, vector<vector<double>> &candles)
{
    cout << "DEBUT ACHAT" << endl;
    unique_ptr<Order> newOrder(new Order());
    double walletUsd = 0;
    this->wallets("USD", walletUsd);
    int i = 0;
    do {
        this->candles(candles, Bitfinex::ASK_LAST_CANDLES);
        double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
        double price = lastClose + 0.1;

        newOrder->setBtxPrice(lastClose);
        newOrder->setPrice(price);
        newOrder->setAmount((walletUsd * 0.2) / price); // on achéte pour 20% du wallet usd

        if (newOrder->getBtxId() == "") {
            cout << "Action : submit" << endl;
            this->submit(newOrder);
        } else if (newOrder->getStatus() == "ACTIVE") {
            // Update lastClose +0.1
            cout << "Action : update" << endl;
            this->update(newOrder);
        }

        this->status(newOrder);
        cout << " Tentative lastClose : " << lastClose << " prix : " << newOrder->getPrice() << " statut : " << newOrder->getStatus() << endl;

        usleep(1000000);
        i++;
    } while(newOrder->getStatus() != "EXECUTED" && i < 10);

    // si au bout de 4 tentatives l'order n'a pas été acheté, on cancel
    if (newOrder->getStatus() != "EXECUTED") {
        this->cancel(newOrder);
        newOrder->setStatus("CANCEL");
        cout << "CANCEL" << endl;
    } else {
        order = move(newOrder);
        cout << "EXECUTED" << endl;
    }

    cout << "FIN ACHAT" << endl;
}


void Bitfinex::sale(unique_ptr<Order> &order, vector<vector<double>> &candles)
{
    cout << "DEBUT VENTE" << endl;
    unique_ptr<Order> newOrder(new Order());
    double walletBtc = 0;
    wallets("BTC", walletBtc);

    int i = 0;
    do {
        this->candles(candles, Bitfinex::ASK_LAST_CANDLES);

        double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
        double fees = walletBtc * 0.002;

        newOrder->setBtxPrice(lastClose);
        newOrder->setPrice(lastClose - 0.1); // Vente lastClose - 0.1
        newOrder->setAmount((walletBtc - fees) * -1); // Montant négatif pour un ordre de vente

        if (newOrder->getBtxId() == "") {
            cout << " Action : submit" << endl;
            this->submit(newOrder);
        } else if(newOrder->getStatus() == "ACTIVE") {
            cout << "Action : update" << endl;
            // Update lastClose  - 0.1
            this->update(newOrder);
        }

        if (newOrder->getStatus() != "error" || newOrder->getCode() != 10114) { // 10114 est l'erreur nonce: small
            this->status(newOrder);
            cout << " Tentative lastClose : " << lastClose << " prix : " << newOrder->getPrice() << " statut : " << newOrder->getStatus() << endl;
        }


        usleep(1000000);
        i++;
    } while(newOrder->getStatus() != "EXECUTED" && i < 10);

    // L'ordre n'est pas passé on le supprime sur btx
    if (newOrder->getStatus() == "error" && newOrder->getCode() != 10114) {
        order.reset();
        cout << "statut ERROR" << endl;
    } else if (newOrder->getStatus() != "EXECUTED") {
        this->cancel(newOrder);
        newOrder->setStatus("CANCEL");
        cout << "statut CANCEL" << endl;
    } else {
        order.reset();
        cout << "statut EXECUTED" << endl;
    }

    cout << "FIN VENTE" << endl;
}
