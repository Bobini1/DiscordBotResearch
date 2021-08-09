#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include "ConnectionManager.h"

using namespace web;
using namespace web::websockets::client;

int main()
{
    ConnectionManager bot(L"");
    while (true);
}
