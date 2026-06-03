#include "hackerbot/protocol/CommandCodec.hpp"
#include "hackerbot/protocol/ResponseParser.hpp"

#include <gtest/gtest.h>

using hackerbot::protocol::CommandCodec;
using hackerbot::protocol::ResponseParser;

// @throws Verifies that empty command names are rejected.
TEST(CommandCodecTest, RejectsEmptyCommandName)
{
    const CommandCodec codec;

    EXPECT_THROW(codec.encode(""), std::invalid_argument);
}

// @post Verifies that commands and arguments are encoded as one serial line.
TEST(CommandCodecTest, EncodesCommandWithArguments)
{
    const CommandCodec codec;

    EXPECT_EQ(codec.encode("MOVE", {"left", "10"}), "MOVE left 10");
}

// @post Verifies that plain-text success responses are parsed as success.
TEST(ResponseParserTest, ParsesTextSuccessResponse)
{
    const ResponseParser parser;

    const auto response = parser.parse("OK:ready");

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "ready");
}

// @post Verifies that error responses preserve the failure message.
TEST(ResponseParserTest, ParsesTextFailureResponse)
{
    const ResponseParser parser;

    const auto response = parser.parse("ERR:device missing");

    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.message, "device missing");
}

// @throws Verifies that malformed responses are rejected.
TEST(ResponseParserTest, RejectsMalformedResponses)
{
    const ResponseParser parser;

    EXPECT_THROW(parser.parse(""), std::invalid_argument);
    EXPECT_THROW(parser.parse("maybe"), std::invalid_argument);
}

// @post Verifies that compact JSON success responses are parsed as success.
TEST(ResponseParserTest, ParsesJsonSuccessResponse)
{
    const ResponseParser parser;

    const auto response = parser.parse("{\"ok\":true,\"payload\":\"ready\"}");

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "ready");
}