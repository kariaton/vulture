#include <iostream>
#include <vector>
#include <unistd.h>

#include "Bitfinex.h"
#include "Util.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Order.h"
#include "Mysql.h"
#include "Wallet.h"

using namespace std;

int main()
{
    cout << "INIT VULTURE" << endl;

    // Trading
    Indicator indicator;
    Bitfinex bitfinex;
    unique_ptr<Order> order;
    bool bbandBottomTouched = false;

    // Generate datas
    vector <vector<double>> candles = {{0}, {0}, {0}, {0}, {0}, {0}};

    // Bdd
    //Mysql mysql;

    // Wallet
    //Wallet wallet;
    //mysql.getWallet(wallet);

//    bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);
//    unique_ptr<Order> newOrder(new Order());
//    newOrder->setBtxId("41679248615");
//
//    bitfinex.status(newOrder);
//    cout << newOrder->getStatus() << endl;

    for(;;) {
        try {
            bitfinex.candles(candles, Bitfinex::ASK_ALL_CANDLES);

            /**INDICATEUR**/
            indicator.bband(candles[Bitfinex::candleOCHL::CLOSE]);

            // On attand que le close touche la bband du
            if (indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && !order && !bbandBottomTouched) {
                bbandBottomTouched = true;
            }

            // Si la bband à été touchée mais est remontée au dessus de la bb il faut acheter.
            if (bbandBottomTouched && !order && !indicator.bbandBottomIsTouched(candles[Bitfinex::candleOCHL::CLOSE])) {
                cout << "DEBUT ACHAT" << endl;
                bbandBottomTouched = false;
                unique_ptr<Order> newOrder(new Order());
                double walletUsd = 0;
                bitfinex.wallets("USD", walletUsd);
                int i = 0;
                do {
                    bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                    double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];
                    double price = lastClose + 0.1;

                    newOrder->setBtxPrice(lastClose);
                    newOrder->setPrice(price);
                    newOrder->setAmount((walletUsd * 0.2) / price); // on achéte pour 20% du wallet usd

                    if (newOrder->getBtxId() == "") {
                        bitfinex.submit(newOrder);
                    } else if (newOrder->getStatus() == "ACTIVE") {
                        // Update lastClose +0.1
                        bitfinex.update(newOrder);
                    }

                    bitfinex.status(newOrder);
                    cout << " Tentative lastClose : " << lastClose << " price " << newOrder->getPrice() << endl << " statut " << newOrder->getStatus() << endl;


                    usleep(1000000);
                    i++;
               } while(newOrder->getStatus() != "EXECUTED" && i < 4);

                // si au bout de 4 tentatives l'order n'a pas été acheté, on cancel
                if (newOrder->getStatus() != "EXECUTED") {
                    bitfinex.cancel(newOrder);
                    newOrder->setStatus("CANCEL");
                    cout << "statut CANCEL" << endl;
                } else {
                    order = move(newOrder);
                    cout << "statut EXECUTED" << endl;
                }
                cout << "FIN ACHAT" << endl;
            }

            // Si la bband du haut est touchée et qu'il y'a un order on vend
            if (indicator.bbandUpperIsTouched(candles[Bitfinex::candleOCHL::CLOSE]) && order) {
                bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];

                if (lastClose > order->getPrice()) {
                    cout << "DEBUT VENTE" << endl;
                    unique_ptr<Order> newOrder(new Order());

                    int i = 0;
                    do {
                        bitfinex.candles(candles, Bitfinex::ASK_LAST_CANDLES);
                        double lastClose = candles[Bitfinex::candleOCHL::CLOSE][0];

                        newOrder->setBtxPrice(lastClose);
                        newOrder->setPrice(lastClose - 0.1); // Vente lastClose - 0.1
                        newOrder->setAmount(order->getAmount() * -1); // Montant négatif pour un ordre de vente

                        if (newOrder->getBtxId() == "") {
                            bitfinex.submit(newOrder);
                        } else if(newOrder->getStatus() == "ACTIVE") {
                            // Update lastClose  - 0.1
                            bitfinex.update(newOrder);
                        }

                        bitfinex.status(newOrder);
                        cout << " Tentative lastClose : " << lastClose << " price " << newOrder->getPrice() << endl << " statut " << newOrder->getStatus() << endl;

                        usleep(1000000);
                        i++;
                    } while(newOrder->getStatus() != "EXECUTED" && i < 4);

                    // L'ordre n'est pas passé on le supprime sur btx
                    if (newOrder->getStatus() != "EXECUTED") {
                        bitfinex.cancel(newOrder);
                        newOrder->setStatus("CANCEL");
                        cout << "statut CANCEL" << endl;
                    } else {
                        order.reset();
                        cout << "statut EXECUTED" << endl;
                    }

                    cout << "FIN VENTE" << endl;
                }
            }
        } catch (string const &e) {
            cerr << e << endl;
        }

        usleep(2000000);
    }

    return 0;
}
