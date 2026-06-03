#ifndef HACKERBOT_TESTS_UNIT_SERIALTRANSPORTTESTDOUBLES_HPP
#define HACKERBOT_TESTS_UNIT_SERIALTRANSPORTTESTDOUBLES_HPP

#include "hackerbot/transport/SerialPortConfig.hpp"
#include "hackerbot/transport/SerialTransport.hpp"

#include <cstddef>
#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace hackerbot
{
    namespace test
    {

        class FakeSerialTransportBackend final : public transport::SerialTransportBackend
        {
        public:
            void open(const transport::SerialPortConfig &aConfig) override
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

            std::deque<transport::SerialPortConfig> openCalls;
            std::deque<std::string> writtenLines;
            std::deque<std::string> readQueue;
            std::size_t closeCalls{0};
            bool openState{false};
        };

        inline std::unique_ptr<FakeSerialTransportBackend> makeFakeSerialTransportBackend()
        {
            return std::make_unique<FakeSerialTransportBackend>();
        }

    } // namespace test
} // namespace hackerbot

#endif