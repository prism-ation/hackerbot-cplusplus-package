#ifndef HACKERBOT_PROTOCOL_RESPONSEPARSER_HPP
#define HACKERBOT_PROTOCOL_RESPONSEPARSER_HPP

#include <string>
#include <string_view>

namespace hackerbot::protocol
{

    /**
     * @brief Parsed response value from the robot protocol.
     * @invariant A success response may carry a message, and a failure response carries an error message.
     */
    struct Response
    {
        bool success{false}; /**< true when the response indicates success. */
        std::string message; /**< Payload on success or error description on failure. */
    };

    /**
     * @brief Parses textual and compact JSON-like response messages.
     * @invariant The parser does not keep mutable state.
     */
    class ResponseParser
    {
    public:
        /**
         * @brief Parses a raw response line.
         * @pre The input is non-empty.
         * @post A valid response object is returned for supported formats.
         * @throws std::invalid_argument when the response is empty or malformed.
         * @param aResponse The raw protocol response.
         * @return The parsed response state and message.
         */
        Response parse(std::string_view aResponse) const;
    };

} // namespace hackerbot::protocol

#endif // HACKERBOT_PROTOCOL_RESPONSEPARSER_HPP