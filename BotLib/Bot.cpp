#include "Bot.h"

namespace Discord
{
	Bot::Bot(std::wstring token, int intents)
		:token(token), intents(intents)
	{
		connector = std::make_shared<Connector>();
		onReady = []() {};
		closeConnection = false;
		finishDestructor = false;
	}

	void Bot::connect()
	{
		heartbeat_interval = connector->connect(token, intents);

		pplx::task<void>([&]()
			{
				srand(time(NULL));
				auto now = std::chrono::high_resolution_clock::now();
				auto firstWait = (((double)rand() / (RAND_MAX))) * heartbeat_interval;

				std::cout << "task" << std::endl;
				std::unique_lock<std::mutex> lock(heartbeatMtx);
				std::cout << "task" << std::endl;

				while (!closeConnection)
				{
					if (finishHeartbeat.wait_until(lock, now + firstWait) == std::cv_status::timeout)
					{
						lock.unlock();
						break;
					}
				}

				while (!closeConnection)
				{
					auto now = std::chrono::high_resolution_clock::now();
					LOG(INFO) << "sending heartbeat" << std::endl;
					connector->sendHearbeat();
					std::unique_lock<std::mutex> lock(heartbeatMtx);
					while (!closeConnection)
					{
						std::cout << "waiting" << std::endl;
						if (finishHeartbeat.wait_until(lock, now + heartbeat_interval) == std::cv_status::timeout) break;
					}
				}
			}).then([&]() { 
				std::unique_lock<std::mutex> lock2(closingMtx);
				finishDestructor = true;
				lock2.unlock();
				disconnected.notify_all();
				});
		onReady();
	}

	std::unordered_map<std::wstring, Guild> Bot::getGuilds()
	{
		return connector->getGuilds();
	}

	Bot::~Bot()
	{
		std::unique_lock<std::mutex> lock(heartbeatMtx);
		closeConnection = true;
		lock.unlock();
		finishHeartbeat.notify_all();
		std::unique_lock<std::mutex> lock2(closingMtx);
		while (!finishDestructor) disconnected.wait(lock2);
	}
}