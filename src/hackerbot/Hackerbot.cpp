#include "hackerbot/Hackerbot.hpp"

namespace hackerbot
{

    namespace
    {

        constexpr std::string_view hackerbotName{"Hackerbot"};

    } // namespace

    Hackerbot::Hackerbot() = default;

    std::string_view Hackerbot::name() const noexcept
    {
        return hackerbotName;
    }

} // namespace hackerbot