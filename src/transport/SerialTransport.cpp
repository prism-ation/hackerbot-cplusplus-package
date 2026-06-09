#include "hackerbot/transport/SerialTransport.hpp"

#include <boost/asio.hpp>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>
#include <utility>

namespace hackerbot::transport
{

    namespace
    {

        class AsioSerialTransportBackend final : public SerialTransportBackend
        {
        public:
            AsioSerialTransportBackend()
                : ioContext(), serialPort(ioContext), timer(ioContext)
            {
            }

            void open(const SerialPortConfig &aConfig) override
            {
                if (aConfig.deviceName.empty())
                {
                    throw std::invalid_argument("serial device name must not be empty");
                }

                if (aConfig.timeout.count() <= 0)
                {
                    throw std::invalid_argument("serial timeout must be positive");
                }

                if (serialPort.is_open())
                {
                    close();
                }

                boost::system::error_code openError;
                serialPort.open(aConfig.deviceName, openError);
                if (openError)
                {
                    throw std::runtime_error("failed to open serial device: " + openError.message());
                }

                /*
                 * a very common failure sequence is:
                 *  1. Pi opens serial port.
                 *  2. Adafruit QT Py resets.
                 *  3. Adafruit QT Py bootloader starts.
                 *  4. Pi immediately writes PING command.
                 *  5. Adafruit QT Py isn't running your sketch yet, so the command is lost.
                 *  6. Adafruit QT Py eventually starts the sketch.
                 *  7. Pi waits forever in: readLine() operation.
                 * To mitigate this, we wait a bit after opening the port to give the Adafruit QT Py time to start up.
                 */

                std::this_thread::sleep_for(std::chrono::seconds(1));

                serialPort.set_option(boost::asio::serial_port_base::baud_rate(aConfig.baudRate));
                serialPort.set_option(
                    boost::asio::serial_port_base::character_size(aConfig.characterSize));
                serialPort.set_option(boost::asio::serial_port_base::parity(toBoostParity(aConfig.parity)));
                serialPort.set_option(boost::asio::serial_port_base::stop_bits(toBoostStopBits(aConfig.stopBits)));
                serialPort.set_option(
                    boost::asio::serial_port_base::flow_control(toBoostFlowControl(aConfig.flowControl)));

                timeout = aConfig.timeout;
            }

            void close() noexcept override
            {
                boost::system::error_code closeError;
                timer.cancel(closeError);
                serialPort.close(closeError);
            }

            bool isOpen() const noexcept override
            {
                return serialPort.is_open();
            }

            void writeLine(const std::string &aLine) override
            {
                if (!serialPort.is_open())
                {
                    throw std::runtime_error("serial transport is not open");
                }

                std::string line = aLine;
                if (line.empty() || line.back() != '\n')
                {
                    line.push_back('\n');
                }

                boost::system::error_code writeError;
                boost::asio::write(serialPort, boost::asio::buffer(line), writeError);
                if (writeError)
                {
                    throw std::runtime_error("failed to write serial line: " + writeError.message());
                }
            }

            std::string readLine() override
            {
                if (!serialPort.is_open())
                {
                    throw std::runtime_error("serial transport is not open");
                }

                boost::asio::streambuf buffer;
                bool timedOut = false;
                bool completed = false;
                boost::system::error_code readError;

                timer.expires_after(timeout);
                timer.async_wait([this, &timedOut](const boost::system::error_code &aError)
                                 {
                                     if (!aError)
                                     {
                                         timedOut = true;
                                         boost::system::error_code cancelError;
                                         serialPort.cancel(cancelError);
                                     } });

                boost::asio::async_read_until(serialPort, buffer, '\n',
                                              [&completed, &readError](const boost::system::error_code &aError, std::size_t)
                                              {
                                                  readError = aError;
                                                  completed = true;
                                              });

                ioContext.restart();
                while (!completed)
                {
                    ioContext.run_one();
                }

                timer.cancel();

                if (timedOut)
                {
                    throw std::runtime_error("serial read timed out");
                }

                if (readError)
                {
                    throw std::runtime_error("failed to read serial line: " + readError.message());
                }

                std::istream inputStream(&buffer);
                std::string line;
                std::getline(inputStream, line);

                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }

                return line;
            }

        private:
            static boost::asio::serial_port_base::parity::type toBoostParity(SerialPortConfig::Parity aParity)
            {
                switch (aParity)
                {
                case SerialPortConfig::Parity::Odd:
                    return boost::asio::serial_port_base::parity::odd;
                case SerialPortConfig::Parity::Even:
                    return boost::asio::serial_port_base::parity::even;
                case SerialPortConfig::Parity::None:
                default:
                    return boost::asio::serial_port_base::parity::none;
                }
            }

            static boost::asio::serial_port_base::stop_bits::type toBoostStopBits(
                SerialPortConfig::StopBits aStopBits)
            {
                switch (aStopBits)
                {
                case SerialPortConfig::StopBits::OneAndHalf:
                    return boost::asio::serial_port_base::stop_bits::onepointfive;
                case SerialPortConfig::StopBits::Two:
                    return boost::asio::serial_port_base::stop_bits::two;
                case SerialPortConfig::StopBits::One:
                default:
                    return boost::asio::serial_port_base::stop_bits::one;
                }
            }

            static boost::asio::serial_port_base::flow_control::type toBoostFlowControl(
                SerialPortConfig::FlowControl aFlowControl)
            {
                switch (aFlowControl)
                {
                case SerialPortConfig::FlowControl::Software:
                    return boost::asio::serial_port_base::flow_control::software;
                case SerialPortConfig::FlowControl::Hardware:
                    return boost::asio::serial_port_base::flow_control::hardware;
                case SerialPortConfig::FlowControl::None:
                default:
                    return boost::asio::serial_port_base::flow_control::none;
                }
            }

            boost::asio::io_context ioContext;
            boost::asio::serial_port serialPort;
            boost::asio::steady_timer timer;
            std::chrono::milliseconds timeout{1000};
        };

    } // namespace

    SerialTransport::SerialTransport(SerialPortConfig aConfig)
        : SerialTransport(std::move(aConfig), std::make_unique<AsioSerialTransportBackend>())
    {
    }

    SerialTransport::SerialTransport(SerialPortConfig aConfig, std::unique_ptr<SerialTransportBackend> aBackend)
        : config(std::move(aConfig)), backend(std::move(aBackend))
    {
        if (!backend)
        {
            throw std::invalid_argument("serial transport backend must not be null");
        }
    }

    void SerialTransport::open()
    {
        validateConfig(config);
        backend->open(config);
    }

    void SerialTransport::close() noexcept
    {
        backend->close();
    }

    bool SerialTransport::isOpen() const noexcept
    {
        return backend->isOpen();
    }

    void SerialTransport::writeLine(std::string_view aLine)
    {
        if (!backend->isOpen())
        {
            throw std::runtime_error("serial transport is not open");
        }

        backend->writeLine(normalizeLine(aLine));
    }

    std::string SerialTransport::readLine()
    {
        if (!backend->isOpen())
        {
            throw std::runtime_error("serial transport is not open");
        }

        return backend->readLine();
    }

    void SerialTransport::validateConfig(const SerialPortConfig &aConfig)
    {
        if (aConfig.deviceName.empty())
        {
            throw std::invalid_argument("serial device name must not be empty");
        }

        if (aConfig.characterSize == 0)
        {
            throw std::invalid_argument("serial character size must be positive");
        }

        if (aConfig.timeout.count() <= 0)
        {
            throw std::invalid_argument("serial timeout must be positive");
        }
    }

    std::string SerialTransport::normalizeLine(std::string_view aLine)
    {
        std::string line(aLine);
        if (line.empty() || line.back() != '\n')
        {
            line.push_back('\n');
        }

        return line;
    }

} // namespace hackerbot::transport