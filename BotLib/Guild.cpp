#include "Guild.h"

Guild::Guild(json::value json)
{
	name = json.at(L"name").as_string();
	memberCount = json.at(L"member_count").as_integer();
	ID = json.at(L"id").as_integer();
	for (json::value channel : json.at(L"channels").as_array())
	{
		channels.push_back(Channel(channel));
	}
}

int Guild::getID()
{
	return ID;
}

int Guild::getMemberCount()
{
	return memberCount;
}

std::wstring Guild::getName()
{
	return name;
}

std::vector<Channel> Guild::getChannels()
{
	return channels;
}

std::vector<Member> Guild::getMembers()
{
	return members;
}
