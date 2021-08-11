#include "pch.h"
#include "../BotLib/ConnectionManager.h"
#include "../BotLib/Guild.h"
#include "../BotLib/Channel.h"
#include <vector>

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(EstablishingConnection, SendToken) {
	std::wfstream tokenStream;
	tokenStream.open("C:/Users/PC/source/repos/token.txt", std::ios::in);
	std::wstring token;
	tokenStream >> token;
	ConnectionManager* bot;
	ASSERT_NO_FATAL_FAILURE(bot =
		new ConnectionManager(token, 0b11111111111111))
		<< "Fatal error encountered while connecting";
	delete bot;
}

TEST(GettingInfo, GettingGuilds) {
	std::wfstream tokenStream;
	tokenStream.open("C:/Users/PC/source/repos/token.txt", std::ios::in);
	std::wstring token;
	tokenStream >> token;
	ConnectionManager* bot = new ConnectionManager(token, 0b11111111111111);
	ASSERT_NE(bot->getGuilds().size(), 0);
	delete bot;
}

TEST(GettingInfo, GettingChannels) {
	std::wfstream tokenStream;
	tokenStream.open("C:/Users/PC/source/repos/token.txt", std::ios::in);
	std::wstring token;
	tokenStream >> token;
	ConnectionManager* bot = new ConnectionManager(token, 0b11111111111111);
	for (Guild guild : bot->getGuilds())
	{
		ASSERT_NE(guild.getChannels().size(), 0);
	}
	delete bot;
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}