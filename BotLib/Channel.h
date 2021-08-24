#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include <string>
#include <unordered_map>
#include "Connector.h"


using namespace web;
using namespace web::websockets::client;
namespace Discord
{
class Connector;

class Channel
{
private:
	std::shared_ptr<Connector> connector;
	std::wstring name;
	int type;
	std::wstring ID;
public:
	Channel(json::value json, std::shared_ptr<Connector> connector);
	std::wstring getName();
	int getType();
	std::wstring getID();
};
}
