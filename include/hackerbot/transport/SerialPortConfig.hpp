#ifndef HACKERBOT_TRANSPORT_SERIALPORTCONFIG_HPP
#define HACKERBOT_TRANSPORT_SERIALPORTCONFIG_HPP

#include <chrono>
#include <string>

namespace hackerbot::transport
{

    /**
     * @brief Configuration for a serial port connection.
     * @invariant The structure is a plain value type; validity is checked by SerialTransport.
     */
    struct SerialPortConfig
    {
        enum class Parity
        {
            None, /**< No parity. */
            Odd,  /**< Odd parity. */
            Even  /**< Even parity. */
        };

        enum class StopBits
        {
            One,        /**< One stop bit. */
            OneAndHalf, /**< One and a half stop bits. */
            Two         /**< Two stop bits. */
        };

        enum class FlowControl
        {
            None,     /**< No flow control. */
            Software, /**< Software flow control. */
            Hardware  /**< Hardware flow control. */
        };

        std::string deviceName;                     /**< Serial device path. */
        unsigned int baudRate{115200};              /**< Baud rate in bits per second. */
        Parity parity{Parity::None};                /**< Serial parity setting. */
        StopBits stopBits{StopBits::One};           /**< Serial stop-bit setting. */
        FlowControl flowControl{FlowControl::None}; /**< Serial flow-control setting. */
        std::chrono::milliseconds timeout{1000};    /**< Read timeout for the backend. */
    };

} // namespace hackerbot::transport

#endif // HACKERBOT_TRANSPORT_SERIALPORTCONFIG_HPP