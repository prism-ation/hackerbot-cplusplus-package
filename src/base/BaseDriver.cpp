#include "hackerbot/base/BaseDriver.hpp"

#include <string>

namespace hackerbot::base
{

    namespace
    {

        constexpr const char *initializeCommand = "B_INIT";
        constexpr const char *startCommand = "B_START";
        constexpr const char *driveCommand = "B_DRIVE";
        constexpr const char *dockCommand = "B_DOCK";
        constexpr const char *statusCommand = "B_STATUS";
        constexpr const char *poseCommand = "B_POSE";
        constexpr const char *mapListCommand = "B_MAPLIST";
        constexpr const char *mapDataCommand = "B_MAPDATA";
        constexpr const char *goToCommand = "B_GOTO";

    } // namespace

    BaseDriver::BaseDriver(hackerbot::transport::SerialTransport &aTransport) : transport(aTransport)
    {
    }

    bool BaseDriver::initialize()
    {
        const hackerbot::protocol::Response response = writeAndRead(initializeCommand);
        return response.success;
    }

    bool BaseDriver::start()
    {
        const hackerbot::protocol::Response response = writeAndRead(startCommand);
        return response.success;
    }

    bool BaseDriver::drive(int aLinearVelocity, int anAngularVelocity)
    {
        const std::string linearVelocity = std::to_string(aLinearVelocity);
        const std::string angularVelocity = std::to_string(anAngularVelocity);

        const hackerbot::protocol::Response response =
            writeAndRead(driveCommand, {linearVelocity, angularVelocity});
        return response.success;
    }

    bool BaseDriver::dock()
    {
        const hackerbot::protocol::Response response = writeAndRead(dockCommand);
        return response.success;
    }

    hackerbot::protocol::Response BaseDriver::status()
    {
        return writeAndRead(statusCommand);
    }

    hackerbot::protocol::Response BaseDriver::pose()
    {
        return writeAndRead(poseCommand);
    }

    hackerbot::protocol::Response BaseDriver::mapList()
    {
        return writeAndRead(mapListCommand);
    }

    hackerbot::protocol::Response BaseDriver::mapData(std::string_view aMapId)
    {
        return writeAndRead(mapDataCommand, {aMapId});
    }

    bool BaseDriver::goTo(double aX, double aY, double anAngle, double aSpeed)
    {
        const std::string xValue = std::to_string(aX);
        const std::string yValue = std::to_string(aY);
        const std::string angleValue = std::to_string(anAngle);
        const std::string speedValue = std::to_string(aSpeed);

        const hackerbot::protocol::Response response =
            writeAndRead(goToCommand, {xValue, yValue, angleValue, speedValue});
        return response.success;
    }

    hackerbot::protocol::Response BaseDriver::writeAndRead(
        const std::string &aCommand,
        std::initializer_list<std::string_view> aArguments)
    {
        transport.writeLine(commandCodec.encode(aCommand, aArguments));
        return responseParser.parse(transport.readLine());
    }

} // namespace hackerbot::base