#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include <string>


using namespace web;
using namespace web::websockets::client;

class Channel
{
private:
	std::wstring name;
	int type;
	std::wstring ID;
public:
	Channel(json::value json);
	std::wstring getName();
	int getType();
	std::wstring getID();
};

