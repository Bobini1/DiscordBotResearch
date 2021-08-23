#include "Guild.h"

Guild::Guild(json::value json, std::shared_ptr<Connector> connector)
{
	name = json.at(L"name").as_string();
	memberCount = json.at(L"member_count").as_integer();
	ID = json.at(L"id").as_string();
	for (json::value channel : json.at(L"channels").as_array())
	{
		std::wstring ID = channel.at(L"id").as_string();
		channels.insert({ ID, Channel(channel, connector) });
	}
}

std::wstring Guild::getID()
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

std::unordered_map<std::wstring, Channel> Guild::getChannels()
{
	if (channels.size() == 0)
	{
		json::value json = connector->request(web::http::methods::GET, L"/guilds/" + ID + L"/channels", {});
		for (json::value channel : json.at(L"channels").as_array())
		{
			std::wstring ID = channel.at(L"id").as_string();
			channels.insert({ ID, Channel(channel, connector) });
		}
	}
	return channels;
}

std::unordered_map<std::wstring, Member> Guild::getMembers()
{
	if (members.size() == 0)
	{
		json::value json = connector->request(web::http::methods::GET, L"/guilds/" + ID + L"/members", L"limit=1000");
		for (json::value member : json.as_array())
		{
			std::wstring ID = member.at(L"id").as_string();
			members.insert({ID, Member(member, connector)});
		}
	}
	return members;
}
