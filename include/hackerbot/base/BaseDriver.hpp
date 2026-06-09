#ifndef HACKERBOT_BASE_BASEDRIVER_HPP
#define HACKERBOT_BASE_BASEDRIVER_HPP

#include "hackerbot/protocol/CommandCodec.hpp"
#include "hackerbot/protocol/ResponseParser.hpp"
#include "hackerbot/transport/SerialTransport.hpp"

#include <initializer_list>
#include <string>
#include <string_view>

namespace hackerbot::base
{

    /**
     * @brief Low-level driver for direct base protocol commands.
     * @invariant The driver never owns the transport; the transport reference remains valid.
     */
    class BaseDriver
    {
    public:
        /**
         * @brief Constructs a base driver bound to a serial transport.
         * @pre The transport reference remains valid for the lifetime of this object.
         * @post The driver is ready to send base commands.
         * @throws None.
         */
        explicit BaseDriver(hackerbot::transport::SerialTransport &aTransport);

        /**
         * @brief Sends the base initialization command.
         * @pre The transport is open.
         * @post Returns true when the robot acknowledges initialization.
         * @throws std::invalid_argument when the response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool initialize();

        /**
         * @brief Sends the base start command.
         * @pre The transport is open.
         * @post Returns true when the robot acknowledges start.
         * @throws std::invalid_argument when the response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool start();

        /**
         * @brief Sends a direct drive command to the base.
         * @pre The transport is open.
         * @post Returns true when the robot acknowledges the drive command.
         * @throws std::invalid_argument when the response is malformed.
         * @param aLinearVelocity Linear velocity in mm/s.
         * @param anAngularVelocity Angular velocity in degrees/s.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool drive(int aLinearVelocity, int anAngularVelocity);

        /**
         * @brief Sends the dock command to the base.
         * @pre The transport is open.
         * @post Returns true when the robot acknowledges docking.
         * @throws std::invalid_argument when the response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool dock();

        /**
         * @brief Queries low-level base status.
         * @pre The transport is open.
         * @post The parsed protocol response is returned.
         * @throws std::invalid_argument when the response is malformed.
         * @return The parsed status response.
         */
        hackerbot::protocol::Response status();

        /**
         * @brief Queries low-level base pose.
         * @pre The transport is open.
         * @post The parsed protocol response is returned.
         * @throws std::invalid_argument when the response is malformed.
         * @return The parsed pose response.
         */
        hackerbot::protocol::Response pose();

        /**
         * @brief Queries the list of maps from the base firmware.
         * @pre The transport is open.
         * @post The parsed protocol response is returned.
         * @throws std::invalid_argument when the response is malformed.
         * @return The parsed map-list response.
         */
        hackerbot::protocol::Response mapList();

        /**
         * @brief Queries map data for a specific map id.
         * @pre The transport is open.
         * @post The parsed protocol response is returned.
         * @throws std::invalid_argument when the response is malformed.
         * @param aMapId The map identifier as understood by the firmware.
         * @return The parsed map-data response.
         */
        hackerbot::protocol::Response mapData(std::string_view aMapId);

        /**
         * @brief Sends a low-level goto command for map navigation.
         * @pre The transport is open.
         * @post Returns true when the robot acknowledges the goto command.
         * @throws std::invalid_argument when the response is malformed.
         * @param aX Target x coordinate in map coordinates.
         * @param aY Target y coordinate in map coordinates.
         * @param anAngle Target heading in degrees.
         * @param aSpeed Target movement speed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool goTo(double aX, double aY, double anAngle, double aSpeed);

    private:
        hackerbot::protocol::Response writeAndRead(
            std::string_view aCommand,
            std::initializer_list<std::string_view> aArguments = {});

        hackerbot::transport::SerialTransport &transport;
        hackerbot::protocol::CommandCodec commandCodec;
        hackerbot::protocol::ResponseParser responseParser;
    };

} // namespace hackerbot::base

#endif // HACKERBOT_BASE_BASEDRIVER_HPP