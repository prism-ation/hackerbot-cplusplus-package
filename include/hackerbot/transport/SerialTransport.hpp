#ifndef HACKERBOT_TRANSPORT_SERIALTRANSPORT_HPP
#define HACKERBOT_TRANSPORT_SERIALTRANSPORT_HPP

#include "hackerbot/transport/SerialPortConfig.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace hackerbot::transport
{

    /**
     * @brief Backend interface for synchronous serial transport operations.
     * @invariant Implementations manage their own open/closed state.
     */
    class SerialTransportBackend
    {
    public:
        virtual ~SerialTransportBackend() = default;

        /**
         * @brief Opens the backend with the provided serial configuration.
         * @pre The configuration has a non-empty device name and a positive timeout.
         * @post The backend is open if no exception is thrown.
         * @throws std::invalid_argument when the configuration is invalid.
         * @throws std::runtime_error when the device cannot be opened.
         */
        virtual void open(const SerialPortConfig &aConfig) = 0;

        /**
         * @brief Closes the backend.
         * @pre None.
         * @post The backend is closed or remains closed.
         * @throws None.
         */
        virtual void close() noexcept = 0;

        /**
         * @brief Checks whether the backend is open.
         * @pre None.
         * @post The backend state is unchanged.
         * @throws None.
         * @return true when the backend is open.
         */
        virtual bool isOpen() const noexcept = 0;

        /**
         * @brief Writes one command line to the backend.
         * @pre The backend is open.
         * @post The line has been sent if no exception is thrown.
         * @throws std::runtime_error when the backend is not open or the write fails.
         */
        virtual void writeLine(const std::string &aLine) = 0;

        /**
         * @brief Reads one response line from the backend.
         * @pre The backend is open.
         * @post A line is returned if no exception is thrown.
         * @throws std::runtime_error when the backend is not open, times out or the read fails.
         * @return The received line without a trailing newline.
         */
        virtual std::string readLine() = 0;
    };

    /**
     * @brief Synchronous serial transport with Boost.Asio under the hood.
     * @invariant The backend pointer is always non-null.
     */
    class SerialTransport
    {
    public:
        /**
         * @brief Creates a transport using the default Boost.Asio backend.
         * @pre The configuration may be default-constructed; validation happens on open().
         * @post The transport is constructed but not opened.
         * @throws std::bad_alloc when allocation of the backend fails.
         */
        explicit SerialTransport(SerialPortConfig aConfig);

        /**
         * @brief Creates a transport with a caller-provided backend.
         * @pre The backend pointer is non-null.
         * @post The transport is constructed but not opened.
         * @throws std::invalid_argument when the backend pointer is null.
         */
        SerialTransport(SerialPortConfig aConfig, std::unique_ptr<SerialTransportBackend> aBackend);

        /**
         * @brief Opens the configured serial transport.
         * @pre The configuration has a non-empty device name and a positive timeout.
         * @post The transport is open if no exception is thrown.
         * @throws std::invalid_argument when the configuration is invalid.
         * @throws std::runtime_error when the backend fails to open the device.
         */
        void open();

        /**
         * @brief Closes the transport.
         * @pre None.
         * @post The transport is closed or remains closed.
         * @throws None.
         */
        void close() noexcept;

        /**
         * @brief Checks whether the transport is open.
         * @pre None.
         * @post The transport state is unchanged.
         * @throws None.
         * @return true when the transport is open.
         */
        [[nodiscard]] bool isOpen() const noexcept;

        /**
         * @brief Writes a single line to the serial device.
         * @pre The transport is open.
         * @post The normalized line has been forwarded to the backend if no exception is thrown.
         * @throws std::runtime_error when the transport is closed or the write fails.
         */
        void writeLine(std::string_view aLine);

        /**
         * @brief Reads a single line from the serial device.
         * @pre The transport is open.
         * @post A line is returned if no exception is thrown.
         * @throws std::runtime_error when the transport is closed, times out or the read fails.
         * @return The received line without a trailing newline.
         */
        std::string readLine();

    private:
        static void validateConfig(const SerialPortConfig &aConfig);
        static std::string normalizeLine(std::string_view aLine);

        SerialPortConfig config;
        std::unique_ptr<SerialTransportBackend> backend;
    };

} // namespace hackerbot::transport

#endif // HACKERBOT_TRANSPORT_SERIALTRANSPORT_HPP