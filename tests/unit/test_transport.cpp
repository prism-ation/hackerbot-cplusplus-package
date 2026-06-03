#include "hackerbot/transport/SerialPortConfig.hpp"
#include "hackerbot/transport/SerialTransport.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <deque>
#include <memory>
#include <stdexcept>
#include <string>

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
            closeCalls += 1;
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
        std::size_t closeCalls{0};
        bool openState{false};
    };

    SerialPortConfig makeValidConfig()
    {
        SerialPortConfig config;
        config.deviceName = "/dev/ttyUSB0";
        config.timeout = std::chrono::milliseconds(250);
        return config;
    }

} // namespace

// @post Verifies that the transport opens the backend with the configured serial settings.
TEST(SerialTransportTest, OpensBackendWithConfiguredDevice)
{
    auto backend = std::make_unique<FakeSerialTransportBackend>();
    auto *backendPointer = backend.get();

    SerialTransport transport(makeValidConfig(), std::move(backend));

    transport.open();

    EXPECT_TRUE(transport.isOpen());
    ASSERT_EQ(backendPointer->openCalls.size(), 1u);
    EXPECT_EQ(backendPointer->openCalls.front().deviceName, "/dev/ttyUSB0");
}

// @post Verifies that writes are normalized with a trailing newline before reaching the backend.
TEST(SerialTransportTest, NormalizesWrittenLines)
{
    auto backend = std::make_unique<FakeSerialTransportBackend>();
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
    auto backend = std::make_unique<FakeSerialTransportBackend>();
    auto *backendPointer = backend.get();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    backendPointer->queueRead("OK:ready");
    transport.open();

    EXPECT_EQ(transport.readLine(), "OK:ready");
}

// @throws Verifies that invalid transport configurations are rejected before opening.
TEST(SerialTransportTest, RejectsInvalidConfiguration)
{
    auto backend = std::make_unique<FakeSerialTransportBackend>();
    SerialTransport transport(SerialPortConfig{}, std::move(backend));

    EXPECT_THROW(transport.open(), std::invalid_argument);
}

// @throws Verifies that write attempts on a closed transport fail.
TEST(SerialTransportTest, RejectsWriteWhenClosed)
{
    auto backend = std::make_unique<FakeSerialTransportBackend>();
    SerialTransport transport(makeValidConfig(), std::move(backend));

    EXPECT_THROW(transport.writeLine("PING"), std::runtime_error);
}