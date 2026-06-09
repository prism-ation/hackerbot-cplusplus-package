#ifndef HACKERBOT_BASE_BASEFACADE_HPP
#define HACKERBOT_BASE_BASEFACADE_HPP

#include "hackerbot/base/BaseDriver.hpp"
#include "hackerbot/base/BaseTypes.hpp"

#include <string_view>

namespace hackerbot::base
{

    /**
     * @brief User-facing facade for base actions above the low-level protocol driver.
     * @invariant The facade never owns the driver; the driver reference remains valid.
     */
    class BaseFacade
    {
    public:
        /**
         * @brief Constructs a base facade on top of a low-level driver.
         * @pre The driver reference remains valid for the lifetime of this object.
         * @post The facade is ready for user-facing base actions.
         * @throws None.
         */
        explicit BaseFacade(BaseDriver &aDriver);

        /**
         * @brief Initializes the base subsystem.
         * @pre The transport backing the driver is open.
         * @post Returns true when initialization was acknowledged.
         * @throws std::invalid_argument when the firmware response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool initialize();

        /**
         * @brief Starts driver mode for movement commands.
         * @pre The transport backing the driver is open.
         * @post On success, driver mode is enabled and the docked flag is cleared.
         * @throws std::invalid_argument when the firmware response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool start();

        /**
         * @brief Drives the base with linear and angular velocity.
         * @pre The transport backing the driver is open.
         * @post On success, the command is acknowledged by the firmware.
         * @throws std::invalid_argument when the firmware response is malformed.
         * @param aLinearVelocity Linear velocity in mm/s.
         * @param anAngularVelocity Angular velocity in degrees/s.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool drive(int aLinearVelocity, int anAngularVelocity);

        /**
         * @brief Docks the base and exits driver mode.
         * @pre The transport backing the driver is open.
         * @post On success, driver mode is disabled and docked becomes true.
         * @throws std::invalid_argument when the firmware response is malformed.
         * @return true on explicit success, false on explicit protocol failure.
         */
        bool dock();

        /**
         * @brief Returns the parsed low-level base status.
         * @pre The transport backing the driver is open.
         * @post A typed status snapshot is returned.
         * @throws std::invalid_argument when status data is malformed or incomplete.
         * @return Parsed base status.
         */
        BaseStatus status();

        /**
         * @brief Returns the parsed base pose.
         * @pre The transport backing the driver is open.
         * @post A typed pose snapshot is returned.
         * @throws std::invalid_argument when pose data is malformed or incomplete.
         * @return Parsed base pose.
         */
        BasePose pose();

        /**
         * @brief Checks whether the facade is currently in driver mode.
         * @pre None.
         * @post State is unchanged.
         * @throws None.
         * @return true when driver mode is enabled.
         */
        [[nodiscard]] bool isDriverMode() const noexcept;

        /**
         * @brief Checks whether the facade currently believes the robot is docked.
         * @pre None.
         * @post State is unchanged.
         * @throws None.
         * @return true when the docked flag is set.
         */
        [[nodiscard]] bool isDocked() const noexcept;

    private:
        static BaseStatus parseStatusPayload(std::string_view aPayload);
        static BasePose parsePosePayload(std::string_view aPayload);

        BaseDriver &driver;
        bool driverMode{false};
        bool docked{true};
    };

} // namespace hackerbot::base

#endif // HACKERBOT_BASE_BASEFACADE_HPP