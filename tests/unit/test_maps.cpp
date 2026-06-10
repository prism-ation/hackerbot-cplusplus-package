#include "SerialTransportTestDoubles.hpp"
#include "hackerbot/base/BaseDriver.hpp"
#include "hackerbot/base/MapsService.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

using hackerbot::base::BaseDriver;
using hackerbot::base::BasePose;
using hackerbot::base::MapsService;
using hackerbot::transport::SerialPortConfig;
using hackerbot::transport::SerialTransport;

// @post Verifies that listMaps parses map ids from firmware payload.
TEST(MapsServiceTest, ListMapsParsesMapIdentifiers)
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
    MapsService maps(driver);

    const auto mapList = maps.listMaps();

    ASSERT_EQ(mapList.size(), 2U);
    EXPECT_EQ(mapList[0].mapId, "home");
    EXPECT_EQ(mapList[1].mapId, "lab");
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_MAPLIST\r\n");
}

// @post Verifies that fetchMapData returns compressed payload for the requested map.
TEST(MapsServiceTest, FetchMapDataReturnsCompressedPayload)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"compressedmapdata\":\"abc123\"}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    MapsService maps(driver);

    const auto mapData = maps.fetchMapData("home");

    EXPECT_EQ(mapData.mapId, "home");
    EXPECT_EQ(mapData.compressedMapData, "abc123");
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_MAPDATA home\r\n");
}

// @post Verifies that goToPose forwards navigation parameters through the low-level driver.
TEST(MapsServiceTest, GoToPoseForwardsNavigationCommand)
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
    MapsService maps(driver);

    BasePose target;
    target.x = 1.0;
    target.y = 2.0;
    target.angle = 90.0;

    EXPECT_TRUE(maps.goToPose(target, 0.2));
    ASSERT_EQ(backendPointer->writtenLines.size(), 1U);
    EXPECT_EQ(backendPointer->writtenLines.front(), "B_GOTO 1.000000 2.000000 90.000000 0.200000\r\n");
}

// @post Verifies that waitUntilReached returns true when pose reaches target within tolerance.
TEST(MapsServiceTest, WaitUntilReachedReturnsTrueAtTarget)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"map_id\":\"home\",\"pose_x\":0.0,\"pose_y\":0.0,\"pose_angle\":0.0}");
    backendPointer->queueRead("OK:{\"map_id\":\"home\",\"pose_x\":1.0,\"pose_y\":1.0,\"pose_angle\":10.0}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    MapsService maps(driver);

    BasePose target;
    target.mapId = "home";
    target.x = 1.0;
    target.y = 1.0;
    target.angle = 10.0;

    EXPECT_TRUE(maps.waitUntilReached(target, 0.1, 2));
    ASSERT_EQ(backendPointer->writtenLines.size(), 2U);
    EXPECT_EQ(backendPointer->writtenLines[0], "B_POSE\r\n");
    EXPECT_EQ(backendPointer->writtenLines[1], "B_POSE\r\n");
}

// @post Verifies that waitUntilReached returns false after exhausting polling budget.
TEST(MapsServiceTest, WaitUntilReachedReturnsFalseWhenTargetNotReached)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"map_id\":\"home\",\"pose_x\":0.0,\"pose_y\":0.0,\"pose_angle\":0.0}");
    backendPointer->queueRead("OK:{\"map_id\":\"home\",\"pose_x\":0.1,\"pose_y\":0.1,\"pose_angle\":1.0}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    MapsService maps(driver);

    BasePose target;
    target.mapId = "home";
    target.x = 5.0;
    target.y = 5.0;
    target.angle = 90.0;

    EXPECT_FALSE(maps.waitUntilReached(target, 0.1, 2));
}

// @throws Verifies that malformed map-list payloads throw an explicit parse error.
TEST(MapsServiceTest, ListMapsThrowsOnMalformedPayload)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:{\"unexpected\":true}");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    BaseDriver driver(transport);
    MapsService maps(driver);

    EXPECT_THROW(maps.listMaps(), std::invalid_argument);
}
