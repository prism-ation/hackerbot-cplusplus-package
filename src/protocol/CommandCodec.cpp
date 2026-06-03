#include "hackerbot/protocol/CommandCodec.hpp"

#include <cctype>
#include <stdexcept>

namespace hackerbot::protocol
{

    namespace
    {

        bool isWhitespaceOnly(std::string_view aText)
        {
            for (const char character : aText)
            {
                if (!std::isspace(static_cast<unsigned char>(character)))
                {
                    return false;
                }
            }

            return true;
        }

    } // namespace

    std::string CommandCodec::encode(
        std::string_view aCommand, std::initializer_list<std::string_view> aArguments) const
    {
        if (aCommand.empty() || isWhitespaceOnly(aCommand))
        {
            throw std::invalid_argument("command name must not be empty");
        }

        std::string encoded(aCommand);
        for (const std::string_view argument : aArguments)
        {
            if (!argument.empty())
            {
                encoded.push_back(' ');
                encoded.append(argument);
            }
        }

        return encoded;
    }

} // namespace hackerbot::protocol