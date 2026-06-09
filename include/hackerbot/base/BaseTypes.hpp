#ifndef HACKERBOT_BASE_BASETYPES_HPP
#define HACKERBOT_BASE_BASETYPES_HPP

#include <cstdint>
#include <string>

namespace hackerbot::base
{

    /**
     * @brief Type-safe position snapshot reported by the base.
     * @invariant Coordinates and angle represent the last parsed pose in base units.
     */
    struct BasePose
    {
        std::string mapId; /**< Identifier of the active map, if reported. */
        double x{0.0};     /**< Position x in map coordinates. */
        double y{0.0};     /**< Position y in map coordinates. */
        double angle{0.0}; /**< Heading angle in degrees. */
    };

    /**
     * @brief Type-safe low-level motion state reported by the base.
     * @invariant Numeric fields reflect one status snapshot from the firmware.
     */
    struct BaseStatus
    {
        std::int64_t timestamp{0}; /**< Status timestamp reported by the firmware. */
        int leftEncoder{0};        /**< Left wheel encoder count. */
        int rightEncoder{0};       /**< Right wheel encoder count. */
        int leftSpeed{0};          /**< Left wheel speed. */
        int rightSpeed{0};         /**< Right wheel speed. */
        int leftSetSpeed{0};       /**< Left wheel target speed. */
        int rightSetSpeed{0};      /**< Right wheel target speed. */
        int wallTof{0};            /**< Wall distance measurement. */
    };

    /**
     * @brief Metadata entry for an available map.
     * @invariant mapId is non-empty for valid map entries.
     */
    struct MapInfo
    {
        std::string mapId; /**< Identifier of the map. */
    };

    /**
     * @brief Map payload returned by map-data queries.
     * @invariant mapId identifies the source map of compressedMapData.
     */
    struct MapData
    {
        std::string mapId;             /**< Identifier of the map. */
        std::string compressedMapData; /**< Serialized compressed map payload. */
    };

} // namespace hackerbot::base

#endif // HACKERBOT_BASE_BASETYPES_HPP