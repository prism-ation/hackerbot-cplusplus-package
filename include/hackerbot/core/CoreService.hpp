#ifndef HACKERBOT_CORE_CORESERVICE_HPP
#define HACKERBOT_CORE_CORESERVICE_HPP

#include "hackerbot/core/RobotState.hpp"
#include "hackerbot/protocol/CommandCodec.hpp"
#include "hackerbot/protocol/ResponseParser.hpp"
#include "hackerbot/transport/SerialTransport.hpp"

#include <string>
#include <string_view>

namespace hackerbot::core
{

    /**
     * @brief Service for querying the robot core and updating the shared state.
     * @invariant The service never owns the transport or the state object.
     */
    class CoreService
    {
    public:
        /**
         * @brief Constructs a core service bound to a transport and shared state.
         * @pre The transport and state references are valid for the lifetime of the service.
         * @post The service is ready to send core commands.
         * @throws None.
         */
        CoreService(hackerbot::transport::SerialTransport &aTransport, RobotState &aState);

        /**
         * @brief Pings the core and updates the attachment flag.
         * @pre The transport is open.
         * @post On success the core attachment flag becomes true; on explicit refusal it becomes false.
         * @throws std::invalid_argument when the response is malformed or missing.
         * @return true when the core reports success, false when it explicitly reports not attached.
         */
        bool ping();

        /**
         * @brief Queries the core version and stores it in the shared state.
         * @pre The transport is open.
         * @post On success the version string is stored and the version attachment flag becomes true.
         * @throws std::invalid_argument when the response is malformed or missing.
         * @throws std::runtime_error when the core explicitly reports a version error.
         * @return The reported version string.
         */
        std::string version();

    private:
        void writeAndReadResponse(std::string_view aCommand,
                                  hackerbot::protocol::Response &aResponse);

        hackerbot::transport::SerialTransport &transport;
        RobotState &state;
        hackerbot::protocol::CommandCodec commandCodec;
        hackerbot::protocol::ResponseParser responseParser;
    };

} // namespace hackerbot::core

#endif // HACKERBOT_CORE_CORESERVICE_HPP