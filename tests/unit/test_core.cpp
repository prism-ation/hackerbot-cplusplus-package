#include "hackerbot/core/CoreService.hpp"
#include "SerialTransportTestDoubles.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <string>

using hackerbot::core::CoreService;
using hackerbot::core::RobotState;
using hackerbot::transport::SerialPortConfig;
using hackerbot::transport::SerialTransport;

// @invariant Verifies that RobotState defaults to a disconnected core without version info.
TEST(RobotStateTest, DefaultsToNotAttachedAndNoVersion)
{
    const RobotState state;

    EXPECT_FALSE(state.coreAttached);
    EXPECT_FALSE(state.versionAttached);
    EXPECT_TRUE(state.version.empty());
}

// @post Verifies that a successful ping marks the core as attached.
TEST(CoreServiceTest, PingMarksCoreAsAttached)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    CoreService service(transport, state);

    EXPECT_TRUE(service.ping());
    EXPECT_TRUE(state.coreAttached);
    EXPECT_FALSE(state.versionAttached);
}

// @post Verifies that an explicit ping refusal clears the attachment flag.
TEST(CoreServiceTest, PingReturnsFalseForExplicitRefusal)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("ERR:core not attached");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    state.coreAttached = true;
    CoreService service(transport, state);

    EXPECT_FALSE(service.ping());
    EXPECT_FALSE(state.coreAttached);
}

// @throws Verifies that malformed ping responses do not silently mutate the state.
TEST(CoreServiceTest, PingRejectsMalformedResponses)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("maybe");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    state.coreAttached = true;
    CoreService service(transport, state);

    EXPECT_THROW(service.ping(), std::invalid_argument);
    EXPECT_TRUE(state.coreAttached);
}

// @post Verifies that a successful version query stores the returned version.
TEST(CoreServiceTest, VersionStoresReturnedVersion)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("OK:1.2.3");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    CoreService service(transport, state);

    EXPECT_EQ(service.version(), "1.2.3");
    EXPECT_TRUE(state.coreAttached);
    EXPECT_TRUE(state.versionAttached);
    EXPECT_EQ(state.version, "1.2.3");
}

// @throws Verifies that explicit version failures surface as runtime errors.
TEST(CoreServiceTest, VersionThrowsForExplicitFailure)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("ERR:version unavailable");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    state.coreAttached = true;
    state.versionAttached = true;
    state.version = "old";
    CoreService service(transport, state);

    EXPECT_THROW(service.version(), std::runtime_error);
    EXPECT_FALSE(state.coreAttached);
    EXPECT_FALSE(state.versionAttached);
    EXPECT_TRUE(state.version.empty());
}

// @throws Verifies that malformed version responses leave the current state untouched.
TEST(CoreServiceTest, VersionRejectsMalformedResponses)
{
    auto backend = hackerbot::test::makeFakeSerialTransportBackend();
    auto *backendPointer = backend.get();
    backendPointer->queueRead("maybe");

    SerialPortConfig config;
    config.deviceName = "/dev/ttyUSB0";
    config.timeout = std::chrono::milliseconds(250);
    SerialTransport transport(config, std::move(backend));
    transport.open();

    RobotState state;
    state.coreAttached = true;
    state.versionAttached = true;
    state.version = "old";
    CoreService service(transport, state);

    EXPECT_THROW(service.version(), std::invalid_argument);
    EXPECT_TRUE(state.coreAttached);
    EXPECT_TRUE(state.versionAttached);
    EXPECT_EQ(state.version, "old");
}