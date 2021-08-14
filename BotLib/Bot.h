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
#include "Channel.h"
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

class Bot
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
	std::atomic<int> sequenceNumber;
	std::vector<Guild> guilds;

	void dispatch(json::value d, int s, json::value t);
	void reconnect(json::value d);
	void invalidSession(json::value d);
	void hello(json::value d);
	void heartbeatACK(json::value d);
	void sendHearbeat();
	void identify();
	void ready(json::value d);
public:
	std::function<void()> onReady;
	Bot();
	~Bot();
	void connect(std::wstring token, int intents);
	std::vector<Guild> getGuilds();
	std::vector<Channel> getChannelsOnGuild(Guild guild);

	std::vector<Channel> getChannelsOnGuild(Guild guild);
	static json::value stringToJson(std::string const& input);
};
