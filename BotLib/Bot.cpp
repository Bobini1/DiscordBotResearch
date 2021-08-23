#include "Bot.h"

Bot::Bot(std::wstring token, int intents)
	:token(token), intents(intents)
{
	connector = std::make_shared<Connector>();
	onReady = []() {};
}

void Bot::connect()
{
	connector->connect(token, intents);
	onReady();
}

std::unordered_map<std::wstring, Guild> Bot::getGuilds()
{
	return connector->getGuilds();
}