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

awaitable<void> tcp_client()
{
    try
    {
        auto executor = co_await boost::asio::this_coro::executor;
        tcp::resolver resolver(executor);
        tcp::socket socket(executor);

        // Server'a bağlan
        auto endpoints = co_await resolver.async_resolve("127.0.0.1", "7878", use_awaitable);
        co_await boost::asio::async_connect(socket, endpoints, use_awaitable);

        std::cout << "Connected to server!" << std::endl;

        // Basit mesaj gönder ve al
        std::string msg = "HELLO\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(msg), use_awaitable);

        char reply[1024];
        std::size_t n = co_await socket.async_read_some(boost::asio::buffer(reply), use_awaitable);

        std::string response(reply, n);
        std::cout << "Server replied: " << response << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main()
{
    boost::asio::io_context io;

    boost::asio::co_spawn(io, tcp_client(), boost::asio::detached);

    io.run();

    return 0;
}
