#ifndef HACKERBOT_BASE_MAPSSERVICE_HPP
#define HACKERBOT_BASE_MAPSSERVICE_HPP

#include "hackerbot/base/BaseDriver.hpp"
#include "hackerbot/base/BaseTypes.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace hackerbot::base
{

    /**
     * @brief Map and navigation workflows layered above the low-level base driver.
     * @invariant The service never owns the driver; the driver reference remains valid.
     */
    class MapsService
    {
    public:
        /**
         * @brief Constructs a map service bound to a low-level base driver.
         * @pre The driver reference remains valid for the lifetime of this object.
         * @post The service is ready for map and pose workflows.
         * @throws None.
         */
        explicit MapsService(BaseDriver &aDriver);

        /**
         * @brief Lists available map identifiers.
         * @pre The transport backing the driver is open.
         * @post Returns zero or more parsed map entries.
         * @throws std::invalid_argument when map-list data is malformed.
         * @return Parsed map entries.
         */
        std::vector<MapInfo> listMaps();

        /**
         * @brief Fetches compressed data for one map.
         * @pre The transport backing the driver is open.
         * @post Returns parsed map data for the requested map id.
         * @throws std::invalid_argument when map-data payload is malformed.
         * @param aMapId Identifier of the target map.
         * @return Parsed map data.
         */
        MapData fetchMapData(std::string_view aMapId);

        /**
         * @brief Sends a navigation goal pose to the firmware.
         * @pre The transport backing the driver is open.
         * @post Returns true when the firmware acknowledges the goto command.
         * @throws std::invalid_argument when the firmware response is malformed.
         * @param aTargetPose Target pose in map coordinates.
         * @param aSpeed Requested movement speed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool goToPose(const BasePose &aTargetPose, double aSpeed);

        /**
         * @brief Reads and returns the current pose.
         * @pre The transport backing the driver is open.
         * @post A typed pose snapshot is returned.
         * @throws std::invalid_argument when pose data is malformed or incomplete.
         * @return Current pose.
         */
        BasePose currentPose();

        /**
         * @brief Polls pose until the target is reached or polling budget is exhausted.
         * @pre The transport backing the driver is open.
         * @post Returns true if target tolerance is met within maxPolls iterations.
         * @throws std::invalid_argument when pose data is malformed or incomplete.
         * @param aTargetPose Goal pose.
         * @param aTolerance Maximum allowed absolute error per axis and angle.
         * @param aMaxPolls Maximum number of pose polls before giving up.
         * @return true when target tolerance is reached, false otherwise.
         */
        bool waitUntilReached(
            const BasePose &aTargetPose,
            double aTolerance = 0.1,
            std::size_t aMaxPolls = 50);

    private:
        static BasePose parsePosePayload(std::string_view aPayload);
        static std::vector<MapInfo> parseMapListPayload(std::string_view aPayload);
        static std::string parseCompressedMapDataPayload(std::string_view aPayload);

        BaseDriver &driver;
    };

} // namespace hackerbot::base

#endif // HACKERBOT_BASE_MAPSSERVICE_HPP