#include "hackerbot/Hackerbot.hpp"

#include <gtest/gtest.h>

// @post Verifies that Hackerbot::name() returns a stable, non-empty package name.
TEST(HackerbotTest, NameReturnsStablePackageName)
{
    const hackerbot::Hackerbot hackerbot;

    EXPECT_EQ(hackerbot.name(), "Hackerbot");
    EXPECT_FALSE(hackerbot.name().empty());
}