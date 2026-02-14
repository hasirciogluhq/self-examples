#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <iostream>
#include <string>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

// client coroutine
awaitable<void> handle_client(tcp::socket socket)
{
    char data[1024];

    try
    {
        while (true)
        {
            boost::system::error_code ec;
            std::size_t n = co_await socket.async_read_some(
                boost::asio::buffer(data),
                use_awaitable);

            // Eğer connection kapandıysa n = 0 dönebilir
            if (n == 0)
            {
                std::cout << "Client closed the connection cleanly." << std::endl;
                break;
            }

            std::string msg(data, n);
            std::cout << "Received: " << msg;

            // Basit switch-case
            if (msg == "PING\n")
            {
                co_await boost::asio::async_write(socket,
                                                  boost::asio::buffer("PONG\n"), use_awaitable);
            }
            else if (msg == "HELLO\n")
            {
                co_await boost::asio::async_write(socket,
                                                  boost::asio::buffer("WORLD\n"), use_awaitable);
            }
            else
            {
                co_await boost::asio::async_write(socket,
                                                  boost::asio::buffer("UNKNOWN\n"), use_awaitable);
            }
        }
    }
    catch (const boost::system::system_error &e)
    {
        if (e.code() == boost::asio::error::eof)
        {
            std::cout << "Client disconnected." << std::endl;
        }
        else
        {
            std::cout << "Client error: " << e.what() << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Client exception: " << e.what() << std::endl;
    }
}

// accept loop coroutine
awaitable<void> server(boost::asio::io_context &io)
{
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 7878));

    while (true)
    {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        boost::asio::co_spawn(io,
                              handle_client(std::move(socket)),
                              boost::asio::detached);
    }
}

int main()
{
    try
    {
        boost::asio::io_context io;

        boost::asio::co_spawn(io,
                              server(io),
                              boost::asio::detached);

        std::cout << "Server listening on port 7878..." << std::endl;
        io.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
    return 0;
}
