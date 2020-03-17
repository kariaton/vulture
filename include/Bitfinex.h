#ifndef BITFINEX_H
#define BITFINEX_H

#include <string>
#include <map>
#include <memory>

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

    void getNonce(std::string &) const;
    void getSignature(std::string const &, std::string const &, std::string const &, std::string &) const;
    void getSig(std::string &, std::string &) const;
    void getBase64(std::string &, std::string &) const;

    void submit(std::unique_ptr<Order> &) const;
    void update(std::unique_ptr<Order> &) const;
    void status(std::unique_ptr<Order> &) const;
    void cancel(std::unique_ptr<Order> &) const;
    void order(std::unique_ptr<Order> &) const;
    void sale(std::unique_ptr<Order> &, std::vector<std::vector<double>> &);
    void buy(std::unique_ptr<Order> &, std::vector<std::vector<double>> &);

    void wallets(std::string const &, double &) const;
    void candles(std::vector<std::vector<double>> &, const bool &) const;

    int get(std::string const &, std::string const &, std::string &) const;
    int post(std::string const &, std::string const &, std::string &) const;

  private:
    std::string _apiKey;
    std::string _apiKeySecret;
    std::string _privateUrl;
    std::string _publicUrl;
    std::string _version;
    std::string _entrypoint;

    Util _util;

    void _init();
};

#endif // BITFINEX_H
