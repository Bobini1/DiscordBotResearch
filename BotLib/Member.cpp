#include "Member.h"

Member::Member(json::value json, std::shared_ptr<Connector> connector)
	:connector(connector)
{
	json::value user = json.at(L"user");
	username = user.at(L"username").as_string();
	ID = user.at(L"id").as_string();
	bot = user.at(L"bot").as_bool();
}

std::wstring Member::getUsername()
{
	return username;
}

std::wstring Member::getID()
{
	return ID;
}

bool Member::isBot()
{
	return bot;
}