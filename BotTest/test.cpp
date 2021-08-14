#include "pch.h"
#include "../BotLib/Bot.h"
#include "../BotLib/Guild.h"
#include "../BotLib/Channel.h"
#include <vector>


class BotTest : public ::testing::Test {
protected:
	std::wstring token;
	Bot* bot;
	void SetUp() override {
		std::wfstream tokenStream;
		tokenStream.open("C:/Users/PC/source/repos/token.txt", std::ios::in);
		tokenStream >> token;
		bot = new Bot();
	}

	void TearDown() override {
		delete bot;
	}
};

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST_F(BotTest, GettingGuilds) {
	bot->onReady = [&]() {
		ASSERT_NE(bot->getGuilds().size(), 0); 
	};
}

TEST_F(BotTest, GettingChannels) {
	bot->onReady = [&]() {
		for (Guild guild : bot->getGuilds())
		{
			ASSERT_NE(guild.getChannels().size(), 0);
		}
	};
	bot->connect(token, 0b11111111111111);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}