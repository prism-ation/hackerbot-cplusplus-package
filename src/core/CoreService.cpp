#include "hackerbot/core/CoreService.hpp"

#include <stdexcept>
#include <utility>

namespace hackerbot::core
{

    namespace
    {

        constexpr const char *pingCommand = "PING";
        constexpr const char *versionCommand = "VERSION";

    } // namespace

    CoreService::CoreService(hackerbot::transport::SerialTransport &aTransport, RobotState &aState)
        : transport(aTransport), state(aState)
    {
    }

    bool CoreService::ping()
    {
        hackerbot::protocol::Response response;
        writeAndReadResponse(pingCommand, response);

        if (response.success)
        {
            state.coreAttached = true;
            return true;
        }

        state.coreAttached = false;
        return false;
    }

    std::string CoreService::version()
    {
        hackerbot::protocol::Response response;
        writeAndReadResponse(versionCommand, response);

        if (!response.success)
        {
            state.coreAttached = false;
            state.versionAttached = false;
            state.version.clear();
            throw std::runtime_error("core version unavailable: " + response.message);
        }

        state.coreAttached = true;
        state.versionAttached = true;
        state.version = response.message;
        return response.message;
    }

    void CoreService::writeAndReadResponse(const std::string &aCommand, hackerbot::protocol::Response &aResponse)
    {
        transport.writeLine(commandCodec.encode(aCommand));
        aResponse = responseParser.parse(transport.readLine());
    }

} // namespace hackerbot::core