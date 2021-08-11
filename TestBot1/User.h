#pragma once

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/http_msg.h>


using namespace web;
using namespace web::websockets::client;

class User
{
	User(json::value json);
};

