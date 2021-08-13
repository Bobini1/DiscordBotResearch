#include "Channel.h"

Channel::Channel(json::value json)
{
	name = json.at(L"name").as_string();
	ID = json.at(L"id").as_string();
	type = json.at(L"type").as_integer();
}

std::wstring Channel::getName()
{
	return name;
}

int Channel::getType()
{
	return type;
}

std::wstring Channel::getID()
{
	return ID;
}
