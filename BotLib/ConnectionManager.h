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
#include "Guild.h"
#include "Channel.h"
#include <cstdio>

using namespace web;
using namespace web::websockets::client;

class ConnectionManager
{
private:
	utility::string_t discordURL;
	websocket_callback_client* client;
	int heartbeat_interval;
	int intents;
	std::wstring token;
	std::atomic<int> sequenceNumber;
	std::vector<Guild> guilds;
	void dispatch(json::value d, int s, json::value t);
	void reconnect(json::value d);
	void invalidSession(json::value d);
	void hello(json::value d);
	void heartbeatACK(json::value d);
	void sendHearbeat();
	void identify();
public:
	ConnectionManager(std::wstring token, int intents);
	~ConnectionManager();
	std::vector<Guild> getGuilds();
	static json::value stringToJson(std::string const& input);
	static std::wstring stringToWstring(std::string const& input);
	static std::string wstringToString(std::wstring const& input);
};
