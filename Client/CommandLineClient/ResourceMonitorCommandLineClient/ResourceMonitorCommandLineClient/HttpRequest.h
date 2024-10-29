#pragma once

#include <iostream>

#include <boost/asio.hpp>

#include "HttpResponse.h"

namespace ResourceMonitorClient::Http {

class Request {
public:
    static const unsigned int DEFAULT_PORT = 80;
public:
    using IoService = boost::asio::io_service;
    using Id = unsigned int;
public:
    Request(IoService& ios, Id id) :
        m_port(DEFAULT_PORT),
        m_id(id),
        mSock(ios),
        m_resolver(ios),
        m_was_cancelled(false),
        m_ios(ios)
    {}

public:
    void set_host(const std::string& host) {
        m_host = host;
    }

    void set_port(unsigned int port) {
        m_port = port;
    }

    void set_uri(const std::string& uri) {
        m_uri = uri;
    }

    std::string get_host() const {
        return m_host;
    }

    unsigned int get_port() const {
        return m_port;
    }

    const std::string& get_uri() const {
        return m_uri;
    }

    unsigned int get_id() const {
        return m_id;
    }

    void execute() {
        assert(m_port > 0);
        assert(m_host.length() > 0);
        assert(m_uri.length() > 0);

        boost::asio::ip::tcp::resolver::query resolver_query(
            m_host, std::to_string(m_port), boost::asio::ip::tcp::resolver::query::numeric_service
        );

        std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        m_resolver.async_resolve(resolver_query, 
            [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator) {
                on_host_name_resolved(ec, iterator);
            }
        );
    }

    void cancel() {
        std::unique_lock<std::mutex>
            cancel_lock(m_cancel_mux);

        m_was_cancelled = true;

        m_resolver.cancel();

        if (mSock.is_open()) {
            mSock.cancel();
        }
    }

private:
    void on_host_name_resolved(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
    {
        if (ec.value() != 0) {
            on_finish(ec);
            return;
        }

        std::unique_lock<std::mutex>
            cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        // Connect to the host.
        boost::asio::async_connect(mSock, iterator,
            [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
            {
                on_connection_established(ec, iterator);
            }
        );

    }

    void on_connection_established(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
    {
        if (ec.value() != 0) {
            on_finish(ec);
            return;
        }

        // Compose the request message.
        m_request_buf += "GET " + m_uri + " HTTP/1.1\r\n";

        // Add mandatory header.
        m_request_buf += "Host: " + m_host + "\r\n";

        m_request_buf += "\r\n";

        std::unique_lock<std::mutex>
            cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        boost::asio::async_write(mSock, boost::asio::buffer(m_request_buf), 
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
            {
                on_request_sent(ec, bytes_transferred);
            }
        );
    }

    void on_request_sent(const boost::system::error_code& ec,
        std::size_t bytes_transferred)
    {
        if (ec.value() != 0) {
            on_finish(ec);
            return;
        }

        mSock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

        std::unique_lock<std::mutex>
            cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        boost::asio::async_read_until(mSock, m_response.getResponseBuf(), "\r\n",
            [this](const boost::system::error_code& ec,
                std::size_t bytes_transferred)
            {
                on_status_line_received(ec, bytes_transferred);
            }
        );
    }

    void on_status_line_received(const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0) {
            on_finish(ec);
            return;
        }

        std::string http_version;
        std::string str_status_code;
        std::string status_message;

        std::istream response_stream(&m_response.getResponseBuf());
        response_stream >> http_version;

        if (http_version != "HTTP/1.1") {
            on_finish(boost::system::error_code());
            return;
        }

        response_stream >> str_status_code;

        unsigned int status_code = 200;

        try {
            status_code = std::stoul(str_status_code);
        }
        catch (std::logic_error&) {
            on_finish(boost::system::error_code());
            return;
        }

        std::getline(response_stream, status_message, '\r');
        // Remove symbol '\n' from the buffer.
        response_stream.get();

        m_response.setStatusCode(status_code);
        m_response.setStatusMessage(status_message);

        std::unique_lock<std::mutex>
            cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        boost::asio::async_read_until(mSock, m_response.getResponseBuf(),
            "\r\n\r\n",
            [this](
                const boost::system::error_code& ec,
                std::size_t bytes_transferred)
            {
                on_headers_received(ec,
                bytes_transferred);
            });
    }

    void on_headers_received(const boost::system::error_code& ec,
        std::size_t bytes_transferred)
    {
        if (ec.value() != 0) {
            on_finish(ec);
            return;
        }

        std::string header, header_name, header_value;
        std::istream response_stream(&m_response.getResponseBuf());

        while (true) {
            std::getline(response_stream, header, '\r');

            // Remove \n symbol from the stream.
            response_stream.get();

            if (header == "")
                break;

            size_t separator_pos = header.find(':');
            if (separator_pos != std::string::npos) {
                header_name = header.substr(0,
                    separator_pos);

                if (separator_pos < header.length() - 1)
                    header_value =
                    header.substr(separator_pos + 1);
                else
                    header_value = "";

                m_response.setHeader(header_name, header_value);
            }
        }

        std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

        if (m_was_cancelled) {
            cancel_lock.unlock();
            on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
            return;
        }

        boost::asio::async_read(mSock, m_response.getResponseBuf(),
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
            {
                on_response_body_received(ec,
                bytes_transferred);
            }
        );

        return;
    }

    void on_response_body_received(const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (ec == boost::asio::error::eof)
            on_finish(boost::system::error_code());
        else
            on_finish(ec);
    }

    void on_finish(const boost::system::error_code& ec)
    {
        if (ec.value() != 0) {
            std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
        }
        else {
            std::cout << m_response.getResponse().rdbuf() << std::endl;
        }

        return;
    }
private:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpResolver = boost::asio::ip::tcp::resolver;
private:
    std::string m_host;
    unsigned int m_port;
    std::string m_uri;

    Id m_id;

    std::string m_request_buf;

    TcpSocket mSock;
    TcpResolver m_resolver;

    Response m_response;

    bool m_was_cancelled;
    std::mutex m_cancel_mux;

    IoService& m_ios;
};

} // namespace ResourceMonitorClient::Http