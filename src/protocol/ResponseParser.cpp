#include "hackerbot/protocol/ResponseParser.hpp"

#include <cctype>
#include <stdexcept>

namespace hackerbot::protocol
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

        bool startsWith(std::string_view aText, std::string_view aPrefix)
        {
            return aText.size() >= aPrefix.size() && aText.substr(0, aPrefix.size()) == aPrefix;
        }

        std::string extractQuotedField(std::string_view aText, std::string_view aFieldName)
        {
            const std::string fieldToken = std::string{"\""} + std::string(aFieldName) + "\":";
            const std::size_t fieldPosition = aText.find(fieldToken);
            if (fieldPosition == std::string_view::npos)
            {
                return {};
            }

            const std::size_t quoteStart = aText.find('"', fieldPosition + fieldToken.size());
            if (quoteStart == std::string_view::npos)
            {
                throw std::invalid_argument("malformed response field");
            }

            const std::size_t quoteEnd = aText.find('"', quoteStart + 1);
            if (quoteEnd == std::string_view::npos)
            {
                throw std::invalid_argument("malformed response field");
            }

            return std::string(aText.substr(quoteStart + 1, quoteEnd - quoteStart - 1));
        }

        Response parseJsonResponse(std::string_view aResponse)
        {
            if (!startsWith(aResponse, "{"))
            {
                throw std::invalid_argument("malformed JSON response");
            }

            Response response;
            const bool hasSuccess = aResponse.find("\"ok\":true") != std::string_view::npos;
            const bool hasFailure = aResponse.find("\"ok\":false") != std::string_view::npos;

            if (hasSuccess == hasFailure)
            {
                throw std::invalid_argument("JSON response must declare a single ok state");
            }

            response.success = hasSuccess;
            if (response.success)
            {
                response.message = extractQuotedField(aResponse, "payload");
            }
            else
            {
                response.message = extractQuotedField(aResponse, "error");
            }

            return response;
        }

        Response parseTextResponse(std::string_view aResponse)
        {
            Response response;
            if (startsWith(aResponse, "OK"))
            {
                response.success = true;
                if (aResponse.size() > 2)
                {
                    std::string_view payload = trim(aResponse.substr(2));
                    if (!payload.empty() && (payload.front() == ':' || payload.front() == '='))
                    {
                        payload.remove_prefix(1);
                    }

                    response.message = std::string(trim(payload));
                }

                return response;
            }

            if (startsWith(aResponse, "ERR"))
            {
                response.success = false;
                if (aResponse.size() > 3)
                {
                    std::string_view message = trim(aResponse.substr(3));
                    if (!message.empty() && (message.front() == ':' || message.front() == '='))
                    {
                        message.remove_prefix(1);
                    }

                    response.message = std::string(trim(message));
                }

                return response;
            }

            throw std::invalid_argument("unsupported response format");
        }

    } // namespace

    Response ResponseParser::parse(std::string_view aResponse) const
    {
        const std::string_view normalizedResponse = trim(aResponse);
        if (normalizedResponse.empty())
        {
            throw std::invalid_argument("response must not be empty");
        }

        if (startsWith(normalizedResponse, "{"))
        {
            return parseJsonResponse(normalizedResponse);
        }

        return parseTextResponse(normalizedResponse);
    }

} // namespace hackerbot::protocol