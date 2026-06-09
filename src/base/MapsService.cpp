#include "hackerbot/base/MapsService.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
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

        double parseDoubleField(std::string_view aPayload, std::string_view aField)
        {
            const std::string raw = extractRawField(aPayload, aField);
            const double value = std::stod(raw);
            return value;
        }

        std::string extractStringArrayRaw(std::string_view aPayload, std::string_view aField)
        {
            const std::string token = std::string{"\""} + std::string(aField) + "\":";
            const std::size_t fieldPosition = aPayload.find(token);
            if (fieldPosition == std::string_view::npos)
            {
                throw std::invalid_argument("missing field: " + std::string(aField));
            }

            const std::size_t arrayStart = aPayload.find('[', fieldPosition + token.size());
            if (arrayStart == std::string_view::npos)
            {
                throw std::invalid_argument("missing array for field: " + std::string(aField));
            }

            const std::size_t arrayEnd = aPayload.find(']', arrayStart + 1);
            if (arrayEnd == std::string_view::npos)
            {
                throw std::invalid_argument("unterminated array for field: " + std::string(aField));
            }

            return std::string(aPayload.substr(arrayStart + 1, arrayEnd - arrayStart - 1));
        }

    } // namespace

    MapsService::MapsService(BaseDriver &aDriver) : driver(aDriver)
    {
    }

    std::vector<MapInfo> MapsService::listMaps()
    {
        const hackerbot::protocol::Response response = driver.mapList();
        if (!response.success)
        {
            throw std::runtime_error("map list unavailable: " + response.message);
        }

        return parseMapListPayload(response.message);
    }

    MapData MapsService::fetchMapData(std::string_view aMapId)
    {
        const hackerbot::protocol::Response response = driver.mapData(aMapId);
        if (!response.success)
        {
            throw std::runtime_error("map data unavailable: " + response.message);
        }

        MapData data;
        data.mapId = std::string(aMapId);
        data.compressedMapData = parseCompressedMapDataPayload(response.message);
        return data;
    }

    bool MapsService::goToPose(const BasePose &aTargetPose, double aSpeed)
    {
        return driver.goTo(aTargetPose.x, aTargetPose.y, aTargetPose.angle, aSpeed);
    }

    BasePose MapsService::currentPose()
    {
        const hackerbot::protocol::Response response = driver.pose();
        if (!response.success)
        {
            throw std::runtime_error("pose unavailable: " + response.message);
        }

        return parsePosePayload(response.message);
    }

    bool MapsService::waitUntilReached(const BasePose &aTargetPose, double aTolerance, std::size_t aMaxPolls)
    {
        for (std::size_t poll = 0; poll < aMaxPolls; ++poll)
        {
            const BasePose current = currentPose();
            const double xOffset = std::fabs(aTargetPose.x - current.x);
            const double yOffset = std::fabs(aTargetPose.y - current.y);
            const double angleOffset = std::fabs(aTargetPose.angle - current.angle);

            if (std::max({xOffset, yOffset, angleOffset}) <= aTolerance)
            {
                return true;
            }
        }

        return false;
    }

    BasePose MapsService::parsePosePayload(std::string_view aPayload)
    {
        BasePose pose;
        pose.mapId = extractRawField(aPayload, "map_id");
        pose.x = parseDoubleField(aPayload, "pose_x");
        pose.y = parseDoubleField(aPayload, "pose_y");
        pose.angle = parseDoubleField(aPayload, "pose_angle");
        return pose;
    }

    std::vector<MapInfo> MapsService::parseMapListPayload(std::string_view aPayload)
    {
        const std::string arrayRaw = extractStringArrayRaw(aPayload, "map_ids");

        std::vector<MapInfo> maps;
        std::size_t cursor = 0;
        while (cursor < arrayRaw.size())
        {
            const std::size_t quoteStart = arrayRaw.find('"', cursor);
            if (quoteStart == std::string::npos)
            {
                break;
            }

            const std::size_t quoteEnd = arrayRaw.find('"', quoteStart + 1);
            if (quoteEnd == std::string::npos)
            {
                throw std::invalid_argument("malformed map_ids array");
            }

            MapInfo map;
            map.mapId = arrayRaw.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            if (!map.mapId.empty())
            {
                maps.push_back(map);
            }

            cursor = quoteEnd + 1;
        }

        return maps;
    }

    std::string MapsService::parseCompressedMapDataPayload(std::string_view aPayload)
    {
        const std::size_t compressedMapDataUnderscore = aPayload.find("\"compressed_map_data\"");
        if (compressedMapDataUnderscore != std::string_view::npos)
        {
            return extractRawField(aPayload, "compressed_map_data");
        }

        return extractRawField(aPayload, "compressedmapdata");
    }

} // namespace hackerbot::base