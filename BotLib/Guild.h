#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include "Channel.h"
#include "Member.h"
#include <vector>

using namespace web;
using namespace web::websockets::client;


class Guild
{
private:
	std::wstring name;
	int memberCount;
	std::wstring ID;
	std::vector<Channel> channels;
	std::vector<Member> members;
public:
	Guild(json::value json);
	std::wstring getID();
	int getMemberCount();
	std::wstring getName();
	std::vector<Channel> getChannels();
	std::vector<Member> getMembers();
};

