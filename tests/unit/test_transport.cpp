#include "hackerbot/transport/SerialPortConfig.hpp"
#include "hackerbot/transport/SerialTransport.hpp"
#include "SerialTransportTestDoubles.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <string>

using hackerbot::transport::SerialPortConfig;
using hackerbot::transport::SerialTransport;

namespace
{

    SerialPortConfig makeValidConfig()
    {
        SerialPortConfig config;
        config.deviceName = "/dev/ttyUSB0";
        config.characterSize = 8;
        config.timeout = std::chrono::milliseconds(250);
        return config;
    }

} // namespace

// @post Verifies that the transport opens the backend with the configured serial settings.
TEST(SerialTransportTest, OpensBackendWithConfiguredDevice)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();

    SerialTransport transport(makeValidConfig(), std::move(backend));

    transport.open();

    EXPECT_TRUE(transport.isOpen());
    ASSERT_EQ(backendPointer->openCalls.size(), 1u);
    EXPECT_EQ(backendPointer->openCalls.front().deviceName, "/dev/ttyUSB0");
    EXPECT_EQ(backendPointer->openCalls.front().characterSize, 8u);
}

// @post Verifies that writes are normalized with a trailing newline before reaching the backend.
TEST(SerialTransportTest, NormalizesWrittenLines)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    transport.open();
    transport.writeLine("PING");

    ASSERT_EQ(backendPointer->writtenLines.size(), 1u);
    EXPECT_EQ(backendPointer->writtenLines.front(), "PING\n");
}

// @post Verifies that reads are forwarded from the backend.
TEST(SerialTransportTest, ReadsLineFromBackend)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    backendPointer->queueRead("OK:ready");
    transport.open();

    EXPECT_EQ(transport.readLine(), "OK:ready");
}

// @post Verifies that closing the transport closes the backend after it has been opened.
TEST(SerialTransportTest, ClosesBackendAfterOpen)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    transport.open();
    transport.close();

    EXPECT_FALSE(transport.isOpen());
    EXPECT_EQ(backendPointer->closeCalls, 1u);
}

// @throws Verifies that invalid transport configurations are rejected before opening.
TEST(SerialTransportTest, RejectsInvalidConfiguration)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    SerialTransport transport(SerialPortConfig{}, std::move(backend));

    EXPECT_THROW(transport.open(), std::invalid_argument);
}

// @throws Verifies that zero character size is rejected before opening.
TEST(SerialTransportTest, RejectsZeroCharacterSize)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    SerialPortConfig config = makeValidConfig();
    config.characterSize = 0;
    SerialTransport transport(config, std::move(backend));

    EXPECT_THROW(transport.open(), std::invalid_argument);
}

// @throws Verifies that write attempts on a closed transport fail.
TEST(SerialTransportTest, RejectsWriteWhenClosed)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    EXPECT_THROW(transport.writeLine("PING"), std::runtime_error);
}