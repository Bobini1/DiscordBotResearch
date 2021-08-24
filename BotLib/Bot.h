#pragma once
#include <memory>
#include "Connector.h"
#include "Guild.h"
#include "Member.h"
#include "Channel.h"
#include <condition_variable>
#include <mutex>


namespace Discord
{

	class Connector;
	class Guild;
	class Channel;
	class Bot
	{
	private:
		std::condition_variable finishHeartbeat;
		std::condition_variable disconnected;
		std::mutex heartbeatMtx;
		std::mutex closingMtx;
		bool finishDestructor;
		bool closeConnection;

		std::shared_ptr<Connector> connector;
		std::wstring token;
		int intents;
		std::unordered_map<std::wstring, Guild> guilds;
		std::chrono::milliseconds heartbeat_interval;
	public:
		std::function<void()> onReady;
		Bot(std::wstring token, int intents);
		~Bot();
		void connect();
		std::unordered_map<std::wstring, Guild> getGuilds();
	};
}