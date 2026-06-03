#include "hackerbot/core/CoreService.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using hackerbot::core::CoreService;
using hackerbot::core::RobotState;
using hackerbot::transport::SerialPortConfig;
using hackerbot::transport::SerialTransport;
using hackerbot::transport::SerialTransportBackend;

namespace
{

    class FakeSerialTransportBackend final : public SerialTransportBackend
    {
    public:
        void open(const SerialPortConfig &aConfig) override
        {
            openCalls.push_back(aConfig);
            openState = true;
        }

        void close() noexcept override
        {
            openState = false;
        }

        bool isOpen() const noexcept override
        {
            return openState;
        }

        void writeLine(const std::string &aLine) override
        {
            if (!openState)
            {
                throw std::runtime_error("backend is closed");
            }

            writtenLines.push_back(aLine);
        }

        std::string readLine() override
        {
            if (!openState)
            {
                throw std::runtime_error("backend is closed");
            }

            if (readQueue.empty())
            {
                throw std::runtime_error("no queued response");
            }

            std::string nextLine = readQueue.front();
            readQueue.pop_front();
            return nextLine;
        }

        void queueRead(std::string aLine)
        {
            readQueue.push_back(std::move(aLine));
        }

        std::deque<SerialPortConfig> openCalls;
        std::deque<std::string> writtenLines;
        std::deque<std::string> readQueue;
        bool openState{false};
    };

    std::unique_ptr<FakeSerialTransportBackend> makeBackend()
    {
        return std::make_unique<FakeSerialTransportBackend>();
    }

} // namespace

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
    auto backend = makeBackend();
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
    auto backend = makeBackend();
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
    auto backend = makeBackend();
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
    auto backend = makeBackend();
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
    auto backend = makeBackend();
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
    auto backend = makeBackend();
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