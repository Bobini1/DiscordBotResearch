#pragma once
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include "Channel.h"
#include <vector>

using namespace web;
using namespace web::websockets::client;
class Member
{
private:
	std::wstring username;
	std::wstring ID;
	bool bot;
public:
	Member(json::value json);
	std::wstring getUsername();
	std::wstring getID();
	bool isBot();
};

