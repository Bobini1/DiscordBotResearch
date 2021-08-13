#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(std::wstring token, int intents)
    :token(token), intents(intents), heartbeat_interval(0)
{
    AixLog::Log::init<AixLog::SinkFile>(AixLog::Severity::trace, "C:/Users/PC/source/repos/TestBot1/logfile.log");

    LOG(INFO) << "Logging!" << std::endl;

    uri_builder builder(L"/gateway/bot");

    http::http_request request(http::methods::GET);

    request.set_request_uri(L"/gateway/bot");
    request.headers().add(L"Authorization", L"Bot " + token);
    http::client::http_client webClient(L"https://discord.com/api/v7");
    try {
        webClient.request(request).then([&](web::http::http_response response)
            {
                LOG(INFO) << "response acquired" << std::endl;
                return response.extract_json();
            }).then([&](json::value json)
                {
                    LOG(INFO) << wstringToString(json.serialize().c_str()) << std::endl;

                    utility::string_t url = json.at(L"url").as_string();
                    discordURL = url;
                }).wait();
    }
    catch (json::json_exception e) { std::wcerr << e.what(); };

    client = new websocket_callback_client;

    websocket_outgoing_message out;
    client->set_message_handler([&](websocket_incoming_message msg)
        {
            LOG(INFO) << "Received a message." << std::endl;
            msg.extract_string().then([&](std::string str)
                {
                    LOG(INFO) << str << std::endl;
                    json::value msgJson = stringToJson(str);
                    int op;
                    json::value d;
                    try {
                        op = msgJson.at(L"op").as_integer();
                        d = msgJson.at(L"d");
                    }
                    catch (const web::json::json_exception& e) {
                        std::wcerr << e.what();
                    }
                    json::value t;
                    switch (op)
                    {
                        case 0:
                            try {
                                sequenceNumber = msgJson.at(L"s").as_integer();
                                t = msgJson.at(L"t");
                                dispatch(d, sequenceNumber, t);
                            }
                            catch (const web::json::json_exception& e){
                                std::wcerr << e.what();
                            }
                            break;
                        case 7:
                            reconnect(d);
                            break;
                        case 9:
                            invalidSession(d);
                            break;
                        case 10:
                            hello(d);
                            break;
                        case 11:
                            heartbeatACK(d);
                    }
                });
        });
    
    try {
        client->connect(uri_builder(discordURL).set_query(L"v=7&encoding=json").to_string()).then([]()
            { /* Successfully connected. */ });
    }
    catch (websocket_exception e) { LOG(ERROR) << e.what() << std::endl; }
}

ConnectionManager::~ConnectionManager()
{
    client->close().then([]() { std::wcout << L"Closed the connection." << std::endl; });
}

void ConnectionManager::dispatch(json::value d, int s, json::value t)
{
    LOG(INFO) << "dispatch type: " << wstringToString(t.serialize().c_str()) << std::endl;
    if (t.as_string() == L"READY")
    {

    }
    else if (t.as_string() == L"GUILD_CREATE") 
    {
        guilds.push_back(Guild(d));
    }
}

void ConnectionManager::reconnect(json::value d)
{
    LOG(INFO) << wstringToString(d.serialize().c_str()) << std::endl;
}

void ConnectionManager::invalidSession(json::value d)
{
    LOG(ERROR) << "Invalid session: " << wstringToString(d.serialize().c_str()) << std::endl;
}

void ConnectionManager::hello(json::value d)
{
    LOG(INFO) << "Hello: " << wstringToString(d.serialize().c_str()) << std::endl;
    heartbeat_interval = d.at(L"heartbeat_interval").as_integer();
    std::chrono::milliseconds interval(heartbeat_interval);
    LOG(INFO) << "interval: " << interval.count() << std::endl;

    pplx::task<void>([=]()
        {
            srand(time(NULL));
            //((((double)rand() / (RAND_MAX)) + 1) * interval) / 2 * 2;
            auto start = std::chrono::high_resolution_clock::now();
            auto firstWait = (((double)rand() / (RAND_MAX))) * interval;
            // std::wcout << L"Waiting for: " << firstWait.count() << std::endl;
            // while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < firstWait.count());

            pplx::wait(((((double)rand() / (RAND_MAX))) * interval).count());
            while(true)
            {
                auto start = std::chrono::high_resolution_clock::now();
                LOG(INFO) << "sending heartbeat" << std::endl;
                sendHearbeat();
                //while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < interval.count());
                pplx::wait(interval.count());
            }
        });

    try {
        identify();
    }
    catch (const web::websockets::client::websocket_exception& e) {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void ConnectionManager::heartbeatACK(json::value d)
{
    LOG(INFO) << "Heartbeat acknowledged: " << wstringToString(d.serialize().c_str()) << std::endl;
}

std::vector<Guild> ConnectionManager::getGuilds()
{
    return guilds;
}

json::value ConnectionManager::stringToJson(std::string const& input) {
    utility::stringstream_t s;
    s << stringToWstring(input);
    json::value ret = json::value::parse(s);
    return ret;
}

std::wstring ConnectionManager::stringToWstring(std::string const& input) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(input);
    return wide;
}

std::string ConnectionManager::wstringToString(std::wstring const& input) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrow = converter.to_bytes(input);
    return narrow;
}

void ConnectionManager::sendHearbeat() {
    websocket_outgoing_message out;
    json::value heartbeat;
    heartbeat[L"op"] = 1;
    heartbeat[L"d"] = sequenceNumber.load();
    LOG(INFO) << "sequenceNumber: " << sequenceNumber.load() << std::endl;
    out.set_utf8_message(wstringToString(heartbeat.serialize().c_str()));
    try {
        client->send(out);
    }
    catch (const web::websockets::client::websocket_exception& e) {
        LOG(ERROR) << "exception!! " << e.what() << std::endl;
    }
}

void ConnectionManager::identify() {
    websocket_outgoing_message out;
    json::value properties;
    properties[L"$os"] = json::value::string(L"Windows 10");
    properties[L"$browser"] = json::value::string(L"TestBot1");
    properties[L"$device"] = json::value::string(L"TestBot1");

    json::value d;
    d[L"token"] = json::value::string(utility::conversions::to_utf16string(this->token));
    d[L"intents"] = intents;
    d[L"properties"] = properties;

    json::value identity;
    identity[L"op"] = 2;
    identity[L"d"] = d;

    out.set_utf8_message(wstringToString(identity.serialize().c_str()));
    try {
        client->send(out).wait();
    }
    catch (const web::websockets::client::websocket_exception& e) {
        LOG(ERROR) << "exception!! " << e.what() << std::endl;
    }
}