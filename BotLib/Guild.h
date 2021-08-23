#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>
#include "Channel.h"
#include "Member.h"
#include "Connector.h"
#include <vector>
#include <unordered_map>

using namespace web;
using namespace web::websockets::client;

class Connector;
class Channel;
class Member;

class Guild
{
private:
	std::shared_ptr<Connector> connector;
	std::wstring name;
	int memberCount;
	std::wstring ID;
	std::unordered_map<std::wstring, Channel> channels;
	std::unordered_map<std::wstring, Member> members;
public:
	Guild(json::value json, std::shared_ptr<Connector> connector);
	std::wstring getID();
	int getMemberCount();
	std::wstring getName();
	std::unordered_map<std::wstring, Channel> getChannels();
	std::unordered_map<std::wstring, Member> getMembers();
};

