#include "hackerbot/core/CoreService.hpp"
#include "hackerbot/core/RobotState.hpp"
#include "hackerbot/transport/SerialPortConfig.hpp"
#include "hackerbot/transport/SerialTransport.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>

namespace
{

    constexpr const char *defaultDeviceName = "/dev/ttyUSB0";
    constexpr int successExitCode = 0;
    constexpr int failureExitCode = 1;

    hackerbot::transport::SerialPortConfig makeConfig(const std::string &aDeviceName)
    {
        hackerbot::transport::SerialPortConfig config;
        config.deviceName = aDeviceName;
        config.timeout = std::chrono::milliseconds(1000);
        return config;
    }

} // namespace

int main(int argc, char *argv[])
{
    try
    {
        const std::string deviceName = argc > 1 ? argv[1] : defaultDeviceName;

        hackerbot::transport::SerialTransport transport(makeConfig(deviceName));
        transport.open();

        hackerbot::core::RobotState robotState;
        hackerbot::core::CoreService coreService(transport, robotState);

        const bool pingOk = coreService.ping();
        std::cout << "ping: " << (pingOk ? "ok" : "not attached") << '\n';

        if (pingOk)
        {
            const std::string version = coreService.version();
            std::cout << "version: " << version << '\n';
        }

        return successExitCode;
    }
    catch (const std::exception &exception)
    {
        std::cerr << "core check failed: " << exception.what() << '\n';
        std::cerr << "usage: " << argv[0] << " [serial-device]" << '\n';
        return failureExitCode;
    }
}