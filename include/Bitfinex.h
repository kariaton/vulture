#ifndef BITFINEX_H
#define BITFINEX_H

#include <string>
#include <map>
#include <Util.h>
#include <Order.h>

class Bitfinex
{
  static size_t writeCallback(void *, size_t, size_t, void *) noexcept;

  public:
    Bitfinex();
    virtual ~Bitfinex();

    static constexpr int ASK_LAST_CANDLES = 1;
    static constexpr int ASK_ALL_CANDLES = 0;
    static constexpr int CANDLES_NUMBER_ELEMENT = 10000; // Nombre de chandelles retournées par l'api
    static constexpr auto CURL_TIMEOUT = 30L;
    static constexpr auto CURL_DEBUG_VERBOSE = 0L;

    enum candleOCHL {MTS, OPEN, CLOSE, HIGH, LOW, VOLUME}; // Enumération pour OCHL (Millisecond, Open, Close, High, Low, Volume)

    std::string getName() const {return _name;}

    void getNonce(std::string &) const;
    void getSignature(std::string const &, std::string const &, std::string const &, std::string &) const;
    void getSig(std::string &, std::string &) const;
    void getBase64(std::string &, std::string &) const;

    void submit(std::string const &, std::string const &, Order &) const;
    void update(Order &) const;
    void cancel(Order &) const;

    void wallets(std::string &) const;
    void candles(std::vector<std::vector<double>> &, const bool &) const;

    int get(std::string const &, std::string const &, std::string &) const;
    int post(std::string const &, std::string const &, std::string &) const;

  private:
    std::string const _name = "Bitfinex";
    std::string _apiKey;
    std::string _apiKeySecret;
    std::string const _privateUrl = "https://api.bitfinex.com";
    std::string const _entrypoint = "v2/auth";

    Util _util;

    void _init();
};

#endif // BITFINEX_H
