#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include <chrono>
#include <locale>
#include <codecvt>
#include <string>
#include <atomic>
#include <vector>

using namespace web;
using namespace web::websockets::client;

class ConnectionManager
{
private:
	utility::string_t discordURL;
	websocket_callback_client* client;
	int heartbeat_interval;
	std::wstring token;
	std::atomic<int> sequenceNumber;
	void dispatch(json::value d, int s, json::value t);
	void reconnect(json::value d);
	void invalidSession(json::value d);
	void hello(json::value d);
	void heartbeatACK(json::value d);
	void sendHearbeat();
	void identify();
public:
	ConnectionManager(std::wstring token);
	void close();
	static json::value stringToJson(std::string const& input);
	static std::wstring stringToWstring(std::string const& input);
	static std::string wstringToString(std::wstring const& input);
};
