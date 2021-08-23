#pragma once
#include <memory>
#include "Connector.h"
#include "Guild.h"
#include "Member.h"
#include "Channel.h"
class Connector;
class Guild;
class Channel;

class Bot
{
private:
	std::shared_ptr<Connector> connector; 
	std::wstring token;
	int intents;
	std::unordered_map<std::wstring, Guild> guilds;
public:
	std::function<void()> onReady;
	Bot(std::wstring token, int intents);
	void connect();
	std::unordered_map<std::wstring, Guild> getGuilds();
};

