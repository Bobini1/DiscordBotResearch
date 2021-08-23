#pragma once
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include <chrono>
#include <locale>
#include <string>
#include <atomic>
#include <vector>
#include "Guild.h"
#include "Member.h"
#include "aixlog.hpp"
#include <cstdio>
#include <iostream>
#include <latch>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>


using namespace web;
using namespace web::websockets::client;

class Guild;
class Member;

class Connector : public std::enable_shared_from_this<Connector>
{
private:
	std::condition_variable readyWait;
	std::mutex readyMtx;
	std::latch* guildsCompleted;
	bool readyReceived;

	utility::string_t discordURL;
	websocket_callback_client* client;
	http::client::http_client* webClient;
	int heartbeat_interval;
	int intents;
	size_t targetNumGuilds;
	std::wstring token;
	std::wstring sessionID;
	std::atomic_int sequenceNumber;
	std::vector<json::value> guilds;

	void dispatch(json::value d, int s, json::value t);
	void reconnect(json::value d);
	void invalidSession(json::value d);
	void hello(json::value d);
	void heartbeatACK(json::value d);
	void sendHearbeat();
	void identify();
	void ready(json::value d);
public:
	Connector();
	void handleWebsocketMessage(const websocket_incoming_message& msg);
	~Connector();
	void connect(std::wstring token, int intents);
	static json::value stringToJson(std::string const& input);
	json::value request(http::method method, std::wstring route, std::wstring query = L"", json::value json = {});
	std::unordered_map<std::wstring, Guild> getGuilds();
};
