#ifndef HACKERBOT_HACKERBOT_HPP
#define HACKERBOT_HACKERBOT_HPP

#include <string_view>

namespace hackerbot
{

    /**
     * @brief Minimal public façade for the Hackerbot package.
     * @invariant The reported name is stable and non-empty.
     */
    class Hackerbot
    {
    public:
        /**
         * @brief Constructs a Hackerbot façade.
         * @pre None.
         * @post The object is ready for use.
         * @throws None.
         */
        Hackerbot();

        /**
         * @brief Returns the public package name.
         * @pre None.
         * @post The returned value is stable for the lifetime of the object.
         * @throws None.
         * @return The package name.
         */
        std::string_view name() const noexcept;
    };

} // namespace hackerbot

#endif // HACKERBOT_HACKERBOT_HPP