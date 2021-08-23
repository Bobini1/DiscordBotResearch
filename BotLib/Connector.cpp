#include "Connector.h"

Connector::Connector()
    :intents(0), heartbeat_interval(0), readyReceived(false), targetNumGuilds(0), guildsCompleted(nullptr)
{
    AixLog::Log::init<AixLog::SinkFile>(AixLog::Severity::trace, "C:/Users/PC/source/repos/TestBot1/logfile.log");

    LOG(INFO) << "Logging!" << std::endl;

    client = new websocket_callback_client;
    webClient = new http::client::http_client(L"https://discord.com/api/v7");

    client->set_message_handler([&](const websocket_incoming_message& msg) {this->handleWebsocketMessage(msg); });
}


void Connector::handleWebsocketMessage(const websocket_incoming_message &msg)
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
                LOG(ERROR) << e.what();
            }
            json::value t;
            switch (op)
            {
            case 0:
                try {
                    if (!msgJson.at(L"s").is_null()) sequenceNumber = msgJson.at(L"s").as_integer();
                    if (!msgJson.at(L"t").is_null()) t = msgJson.at(L"t");
                    else t = {};
                    dispatch(d, sequenceNumber, t);
                }
                catch (const web::json::json_exception& e) {
                    LOG(ERROR) << e.what();
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
}


void Connector::connect(std::wstring token, int intents)
{
    this->token = token;
    this->intents = intents;

    discordURL = request(http::methods::GET, L"/gateway/bot").at(L"url").as_string();

    //In case the user tries to connect more than one time.
    if (readyReceived) readyReceived = false;

    try {
        client->connect(uri_builder(discordURL).set_query(L"v=7&encoding=json").to_string()).then([]()
            { /* Successfully connected. */ });
    }
    catch (websocket_exception e) { LOG(ERROR) << e.what() << std::endl; }

    std::unique_lock<std::mutex> lck(readyMtx);
    while (!readyReceived) readyWait.wait(lck);

    guildsCompleted->wait();
    delete guildsCompleted;
}

Connector::~Connector()
{
    client->close().then([]() { std::wcout << L"Closed the connection." << std::endl; });
    delete client;
    delete webClient;
}

void Connector::dispatch(json::value d, int s, json::value t)
{
    LOG(INFO) << "dispatch type: " << utility::conversions::utf16_to_utf8(t.serialize().c_str()) << std::endl;
    if (t.as_string() == L"READY")
    {
        ready(d);
    }
    else if (t.as_string() == L"GUILD_CREATE") 
    {
        guilds.push_back(d);
        guildsCompleted->count_down();
    }
}

void Connector::reconnect(json::value d)
{
    LOG(INFO) << utility::conversions::utf16_to_utf8(d.serialize().c_str()) << std::endl;
}

void Connector::invalidSession(json::value d)
{
    LOG(ERROR) << "Invalid session: " << utility::conversions::utf16_to_utf8(d.serialize().c_str()) << std::endl;
}

void Connector::hello(json::value d)
{
    LOG(INFO) << "Hello: " << utility::conversions::utf16_to_utf8(d.serialize().c_str()) << std::endl;
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

void Connector::heartbeatACK(json::value d)
{
    LOG(INFO) << "Heartbeat acknowledged: " << utility::conversions::utf16_to_utf8(d.serialize().c_str()) << std::endl;
}

json::value Connector::stringToJson(std::string const& input) {
    utility::stringstream_t s;
    s << utility::conversions::utf8_to_utf16(input);
    json::value ret;
    try {
        ret = json::value::parse(s);
    }
    catch (json::json_exception e) { std::wcerr << e.what(); }
    return ret;
}

json::value Connector::request(http::method method, std::wstring route, std::wstring query, json::value json)
{
    http::http_request request(method);
    request.set_request_uri(uri_builder(discordURL).set_path(route).set_query(query).to_string());
    request.headers().add(L"Authorization", L"Bot " + token);

    json::value data;
    try {
        webClient->request(request).then([&](web::http::http_response response)
            {
                LOG(INFO) << "response acquired" << std::endl;
                return response.extract_json();
            }).then([&](json::value json)
                {
                    LOG(INFO) << utility::conversions::utf16_to_utf8(json.serialize().c_str()) << std::endl;
                    data = json;
                }).wait();
    }
    catch (json::json_exception e) { std::wcerr << e.what(); };
    return data;
}

void Connector::sendHearbeat() {
    websocket_outgoing_message out;
    json::value heartbeat;
    heartbeat[L"op"] = 1;
    heartbeat[L"d"] = sequenceNumber.load();
    LOG(INFO) << "sequenceNumber: " << sequenceNumber.load() << std::endl;
    out.set_utf8_message(utility::conversions::utf16_to_utf8(heartbeat.serialize().c_str()));
    try {
        client->send(out);
    }
    catch (const web::websockets::client::websocket_exception& e) {
        LOG(ERROR) << "exception!! " << e.what() << std::endl;
    }
}

void Connector::identify() {
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

    out.set_utf8_message(utility::conversions::utf16_to_utf8(identity.serialize().c_str()));
    try {
        client->send(out).wait();
    }
    catch (const web::websockets::client::websocket_exception& e) {
        LOG(ERROR) << "exception!! " << e.what() << std::endl;
    }
}

void Connector::ready(json::value d)
{
    std::unique_lock<std::mutex> lck(readyMtx);
    targetNumGuilds = d.at(L"guilds").as_array().size();
    guildsCompleted = new std::latch(targetNumGuilds);
    sessionID = d.at(L"session_id").as_string();
    lck.unlock();
    readyReceived = true;
    readyWait.notify_all();
}

std::unordered_map<std::wstring, Guild> Connector::getGuilds()
{
    std::unordered_map<std::wstring, Guild> result;
    for (json::value json : guilds)
    {
        result.insert({ json.at(L"id").as_string(), Guild(json, shared_from_this()) });
    }
    return result;
}
