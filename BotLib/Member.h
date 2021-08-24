#pragma once
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include "Channel.h"
#include "Connector.h"
#include <vector>

using namespace web;
using namespace web::websockets::client;

namespace Discord
{
class Connector;
class Member
{
private:
	std::shared_ptr<Connector> connector;
	std::wstring username;
	std::wstring ID;
	bool bot;
public:
	Member(json::value json, std::shared_ptr<Connector> connector);
	std::wstring getUsername();
	std::wstring getID();
	bool isBot();
};
}

