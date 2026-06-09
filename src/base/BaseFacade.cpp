#include "hackerbot/base/BaseFacade.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace hackerbot::base
{

    namespace
    {

        std::string_view trim(std::string_view aText)
        {
            while (!aText.empty() && std::isspace(static_cast<unsigned char>(aText.front())))
            {
                aText.remove_prefix(1);
            }

            while (!aText.empty() && std::isspace(static_cast<unsigned char>(aText.back())))
            {
                aText.remove_suffix(1);
            }

            return aText;
        }

        std::string extractRawField(std::string_view aPayload, std::string_view aField)
        {
            const std::string token = std::string{"\""} + std::string(aField) + "\":";
            const std::size_t fieldPosition = aPayload.find(token);
            if (fieldPosition == std::string_view::npos)
            {
                throw std::invalid_argument("missing field: " + std::string(aField));
            }

            std::size_t valueStart = fieldPosition + token.size();
            while (valueStart < aPayload.size() &&
                   std::isspace(static_cast<unsigned char>(aPayload[valueStart])))
            {
                valueStart += 1;
            }

            if (valueStart >= aPayload.size())
            {
                throw std::invalid_argument("missing value for field: " + std::string(aField));
            }

            if (aPayload[valueStart] == '"')
            {
                const std::size_t quoteEnd = aPayload.find('"', valueStart + 1);
                if (quoteEnd == std::string_view::npos)
                {
                    throw std::invalid_argument("unterminated string field: " + std::string(aField));
                }

                return std::string(aPayload.substr(valueStart + 1, quoteEnd - valueStart - 1));
            }

            std::size_t valueEnd = valueStart;
            while (valueEnd < aPayload.size() && aPayload[valueEnd] != ',' && aPayload[valueEnd] != '}')
            {
                valueEnd += 1;
            }

            return std::string(trim(aPayload.substr(valueStart, valueEnd - valueStart)));
        }

        int parseIntField(std::string_view aPayload, std::string_view aField)
        {
            const std::string raw = extractRawField(aPayload, aField);
            const int value = std::stoi(raw);
            return value;
        }

        std::int64_t parseInt64Field(std::string_view aPayload, std::string_view aField)
        {
            const std::string raw = extractRawField(aPayload, aField);
            const std::int64_t value = std::stoll(raw);
            return value;
        }

        double parseDoubleField(std::string_view aPayload, std::string_view aField)
        {
            const std::string raw = extractRawField(aPayload, aField);
            const double value = std::stod(raw);
            return value;
        }

    } // namespace

    BaseFacade::BaseFacade(BaseDriver &aDriver) : driver(aDriver)
    {
    }

    bool BaseFacade::initialize()
    {
        return driver.initialize();
    }

    bool BaseFacade::start()
    {
        const bool started = driver.start();
        if (started)
        {
            driverMode = true;
            docked = false;
        }

        return started;
    }

    bool BaseFacade::drive(int aLinearVelocity, int anAngularVelocity)
    {
        if (!driverMode)
        {
            const bool started = start();
            if (!started)
            {
                return false;
            }
        }

        return driver.drive(aLinearVelocity, anAngularVelocity);
    }

    bool BaseFacade::dock()
    {
        const bool dockedSuccessfully = driver.dock();
        if (dockedSuccessfully)
        {
            docked = true;
            driverMode = false;
        }

        return dockedSuccessfully;
    }

    BaseStatus BaseFacade::status()
    {
        const hackerbot::protocol::Response response = driver.status();
        if (!response.success)
        {
            throw std::runtime_error("base status unavailable: " + response.message);
        }

        return parseStatusPayload(response.message);
    }

    BasePose BaseFacade::pose()
    {
        const hackerbot::protocol::Response response = driver.pose();
        if (!response.success)
        {
            throw std::runtime_error("base pose unavailable: " + response.message);
        }

        return parsePosePayload(response.message);
    }

    bool BaseFacade::isDriverMode() const noexcept
    {
        return driverMode;
    }

    bool BaseFacade::isDocked() const noexcept
    {
        return docked;
    }

    BaseStatus BaseFacade::parseStatusPayload(std::string_view aPayload)
    {
        BaseStatus status;
        status.timestamp = parseInt64Field(aPayload, "timestamp");
        status.leftEncoder = parseIntField(aPayload, "left_encoder");
        status.rightEncoder = parseIntField(aPayload, "right_encoder");
        status.leftSpeed = parseIntField(aPayload, "left_speed");
        status.rightSpeed = parseIntField(aPayload, "right_speed");
        status.leftSetSpeed = parseIntField(aPayload, "left_set_speed");
        status.rightSetSpeed = parseIntField(aPayload, "right_set_speed");
        status.wallTof = parseIntField(aPayload, "wall_tof");
        return status;
    }

    BasePose BaseFacade::parsePosePayload(std::string_view aPayload)
    {
        BasePose pose;
        pose.mapId = extractRawField(aPayload, "map_id");
        pose.x = parseDoubleField(aPayload, "pose_x");
        pose.y = parseDoubleField(aPayload, "pose_y");
        pose.angle = parseDoubleField(aPayload, "pose_angle");
        return pose;
    }

} // namespace hackerbot::base