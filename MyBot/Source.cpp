#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <ConnectionManager.h>
#include <fstream>

using namespace web;
using namespace web::websockets::client;

int main()
{
    std::wfstream tokenStream;
    tokenStream.open("C:/Users/PC/source/repos/token.txt", std::ios::in);
    std::wstring token;
    tokenStream >> token;
    ConnectionManager bot(token, 0b11111111111111);
    std::wcout << L"Guilds: " << bot.getGuilds().size() << std::endl;
    pplx::wait(5000);
    std::wcout << L"Guilds: " << bot.getGuilds().size() << std::endl;
    while (true);
}
