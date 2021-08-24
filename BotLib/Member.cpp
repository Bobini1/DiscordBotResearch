#include "Member.h"

namespace Discord
{
	Member::Member(json::value json, std::shared_ptr<Connector> connector)
		:connector(connector)
	{
		json::value user = json.at(L"user");
		username = user.at(L"username").as_string();
		ID = user.at(L"id").as_string();
		if (user.has_field(L"bot")) bot = user.at(L"bot").as_bool();
		else bot = false;
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
}
