#ifndef HACKERBOT_CORE_ROBOTSTATE_HPP
#define HACKERBOT_CORE_ROBOTSTATE_HPP

#include <string>

namespace hackerbot::core
{

    /**
     * @brief Snapshot of the global core state.
     * @invariant The attachment flags reflect the last successfully observed state.
     */
    struct RobotState
    {
        bool coreAttached{false};    /**< true when the core responds to ping. */
        bool versionAttached{false}; /**< true when a version response has been observed. */
        std::string version;         /**< Last known version string reported by the core. */
    };

} // namespace hackerbot::core

#endif // HACKERBOT_CORE_ROBOTSTATE_HPP