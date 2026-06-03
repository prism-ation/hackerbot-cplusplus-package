#ifndef HACKERBOT_PROTOCOL_COMMANDCODEC_HPP
#define HACKERBOT_PROTOCOL_COMMANDCODEC_HPP

#include <initializer_list>
#include <string>
#include <string_view>

namespace hackerbot::protocol
{

    /**
     * @brief Encodes high-level command names and arguments into serial command strings.
     * @invariant The codec does not keep mutable state.
     */
    class CommandCodec
    {
    public:
        /**
         * @brief Encodes a command line.
         * @pre The command name is non-empty and contains no whitespace.
         * @post The returned string contains the command and its arguments separated by spaces.
         * @throws std::invalid_argument when the command name is empty or malformed.
         * @param aCommand The command name.
         * @param aArguments Optional command arguments.
         * @return The encoded command string without a trailing newline.
         */
        std::string encode(std::string_view aCommand,
                           std::initializer_list<std::string_view> aArguments = {}) const;
    };

} // namespace hackerbot::protocol

#endif // HACKERBOT_PROTOCOL_COMMANDCODEC_HPP