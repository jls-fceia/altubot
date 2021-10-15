#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>
#include <thread>

#include <tgbot/tgbot.h>
#include "restc-cpp/restc-cpp.h"

using namespace TgBot;
using namespace restc_cpp;
using namespace std;

void DoSomethingInteresting(Context &ctx)
{
    using std::chrono::system_clock;

    const string token("1973982233:AAHM-L0nj6ZJUvJKfTyqfmHSgp4HNr8lN2o");
    const std::int64_t userid = 895407247;
    Bot bot(token);

    std::string urlINA("https://alerta.ina.gob.ar/pub/datos/datosDia");
    time_t rawtime;
    struct tm *timeinfo;
    string msj;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    int anio = timeinfo->tm_year + 1900;
    std::string fecha(std::to_string(anio) + "-" + std::to_string(timeinfo->tm_mon + 1) +
                      "-" + std::to_string((timeinfo->tm_mday) - 1));

    urlINA += "&date=";
    urlINA += fecha;
    urlINA += "&seriesId=34&siteCode=34&varId=2&procId=1&desc=1&format=json";

    // Asynchronously connect to a INA server and fetch data.
    auto reply = ctx.Get(urlINA);

    // Asynchronously fetch the entire data-set and return it as a string.
    auto json = reply->GetBodyAsString();

    if (json.length() != 0)
    {
        std::string altura;
        int sPos = json.find("valor");
        if (sPos > 0){
            altura = json.substr(sPos + 8, 5);
            msj = "La altura registrada el " + fecha + " es: " + altura + " mts.";
        } else
            msj = "No hay datos de altura para el " + fecha;
    }
    else
        msj = "No se recibieron datos";

    bot.getApi().sendMessage(userid, msj);

    return;
}

int main()
{
    auto rest_client = RestClient::Create();

    // Call DoSomethingInteresting as a co-routine in a worker-thread.
    rest_client->Process(DoSomethingInteresting);

    // Wait for the coroutine to finish, then close the client.
    rest_client->CloseWhenReady(true);
}
