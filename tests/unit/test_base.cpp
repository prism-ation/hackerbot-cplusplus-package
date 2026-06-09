#include "SerialTransportTestDoubles.hpp"
#include "hackerbot/base/BaseFacade.hpp"
#include "hackerbot/base/BaseDriver.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

using hackerbot::base::BaseDriver;
using hackerbot::base::BaseFacade;
using hackerbot::transport::SerialPortConfig;
using hackerbot::transport::SerialTransport;

// @post Returns true when B_INIT receives an explicit success response.
TEST(BaseDriverTest, InitializeReturnsTrueOnSuccess)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);

    EXPECT_TRUE(driver.initialize());
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_INIT");
}

// @post Returns false when B_START receives an explicit protocol failure.
TEST(BaseDriverTest, StartReturnsFalseOnExplicitFailure)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("ERR:cannot start");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);

    EXPECT_FALSE(driver.start());
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_START");
}

// @post Encodes B_DRIVE with linear and angular arguments and reports success.
TEST(BaseDriverTest, DriveEncodesCommandArguments)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:drive accepted");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);

    EXPECT_TRUE(driver.drive(200, -45));
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_DRIVE 200 -45");
}

// @post Returns parsed payload for status responses.
TEST(BaseDriverTest, StatusReturnsParsedPayload)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"left_set_speed\":0,\"right_set_speed\":0}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    const hackerbot::protocol::Response response = driver.status();

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "{\"left_set_speed\":0,\"right_set_speed\":0}");
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_STATUS");
}

// @throws Rejects malformed pose responses.
TEST(BaseDriverTest, PoseRejectsMalformedResponse)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("unknown");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);

    EXPECT_THROW(driver.pose(), std::invalid_argument);
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_POSE");
}

// @post Forwards B_MAPLIST and returns parsed map-list protocol response.
TEST(BaseDriverTest, MapListForwardsCommand)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"map_ids\":[\"home\",\"lab\"]}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    const hackerbot::protocol::Response response = driver.mapList();

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "{\"map_ids\":[\"home\",\"lab\"]}");
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_MAPLIST");
}

// @post Forwards B_MAPDATA with the given map id.
TEST(BaseDriverTest, MapDataForwardsMapId)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"compressedmapdata\":\"abc\"}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    const hackerbot::protocol::Response response = driver.mapData("home");

    EXPECT_TRUE(response.success);
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_MAPDATA home");
}

// @post Forwards B_GOTO with target pose and speed.
TEST(BaseDriverTest, GoToForwardsNavigationCommand)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);

    EXPECT_TRUE(driver.goTo(1.0, 2.0, 90.0, 0.25));
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_GOTO 1.000000 2.000000 90.000000 0.250000");
}

// @post Verifies that drive automatically starts driver mode when needed.
TEST(BaseFacadeTest, DriveStartsDriverModeWhenNeeded)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK");
    backendPointer->queueRead("OK");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    BaseFacade facade(driver);

    EXPECT_TRUE(facade.drive(100, 0));
    EXPECT_TRUE(facade.isDriverMode());
    EXPECT_FALSE(facade.isDocked());
    ASSERT_EQ(backendPointer->writtenLines.size(), 2U);
    EXPECT_EQ(backendPointer->writtenLines[0], "B_START");
    EXPECT_EQ(backendPointer->writtenLines[1], "B_DRIVE 100 0");
}

// @post Verifies that dock disables driver mode and marks the base as docked.
TEST(BaseFacadeTest, DockUpdatesFacadeState)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK");
    backendPointer->queueRead("OK");
    backendPointer->queueRead("OK");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    BaseFacade facade(driver);

    EXPECT_TRUE(facade.start());
    EXPECT_TRUE(facade.drive(30, 5));
    EXPECT_TRUE(facade.dock());
    EXPECT_FALSE(facade.isDriverMode());
    EXPECT_TRUE(facade.isDocked());
}

// @post Verifies that status maps payload fields to BaseStatus.
TEST(BaseFacadeTest, StatusReturnsTypedStatus)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead(
        "OK:{\"timestamp\":123,\"left_encoder\":1,\"right_encoder\":2,\"left_speed\":3,"
        "\"right_speed\":4,\"left_set_speed\":5,\"right_set_speed\":6,\"wall_tof\":7}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    BaseFacade facade(driver);

    const hackerbot::base::BaseStatus status = facade.status();

    EXPECT_EQ(status.timestamp, 123);
    EXPECT_EQ(status.leftEncoder, 1);
    EXPECT_EQ(status.rightEncoder, 2);
    EXPECT_EQ(status.leftSpeed, 3);
    EXPECT_EQ(status.rightSpeed, 4);
    EXPECT_EQ(status.leftSetSpeed, 5);
    EXPECT_EQ(status.rightSetSpeed, 6);
    EXPECT_EQ(status.wallTof, 7);
}

// @throws Verifies that malformed status payloads fail explicitly.
TEST(BaseFacadeTest, StatusThrowsOnMalformedPayload)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"timestamp\":123}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);

    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    BaseFacade facade(driver);

    EXPECT_THROW(facade.status(), std::invalid_argument);
}
