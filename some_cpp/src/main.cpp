#include <memory>
#include <coroutine>
#include <iostream>
#include <string>
#include <random>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/thread.hpp>
#include "database.hpp"
#include <thread>

std::string random_string(size_t length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyz"; // küçük harfler
    std::string result;
    result.resize(length);

    std::random_device rd;  // gerçek random seed
    std::mt19937 gen(rd()); // mersenne twister
    std::uniform_int_distribution<> dist(0, 25);

    for (size_t i = 0; i < length; ++i)
        result[i] = charset[dist(gen)];

    return result;
}

// 2. Coroutine Fonksiyonumuz!
boost::asio::awaitable<void> benimCoroutine(boost::asio::io_context &io)
{
    std::println("rutin start yedi;");
    for (size_t i = 0; i < 5; i++)
    {

        SDatabase *instance = SDatabase::getInstance();

        instance->set("coroutine_key", "sex");

        const std::string result = instance->get("coroutine_key");

        boost::asio::steady_timer timer(io, std::chrono::milliseconds(20));
        co_await timer.async_wait(boost::asio::use_awaitable); // non-blocking wait
    }
    std::printf("   [Coroutine]: Done!\n");

    co_return;
}

void baseThreadRunner(boost::asio::io_context &io_context)
{
    io_context.run();
}

int main()
{
    boost::asio::io_context io_context;
    auto work = boost::asio::make_work_guard(io_context);

    SDatabase *db = SDatabase::getInstance();
    const auto start = std::chrono::steady_clock::now();
    std::cout << "Main: Coroutine'i tanimliyorum...\n";
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);
    boost::asio::co_spawn(io_context, benimCoroutine(io_context), boost::asio::detached);

    boost::thread thread(baseThreadRunner, std::ref(io_context));
    boost::thread thread1(baseThreadRunner, std::ref(io_context));
    boost::thread thread2(baseThreadRunner, std::ref(io_context));
    boost::thread thread3(baseThreadRunner, std::ref(io_context));

    std::string lastKey = "";
    int max = 100000;
    for (size_t i = 0; i <= max; i++)
    {
        std::string key = random_string(10);
        std::string value = random_string(100);
        db->set(std::move(key), std::move(value));

        if (i == 44879)
        {
            lastKey = key;
        }
    }

    if (!lastKey.empty())
    {

        const auto start = std::chrono::steady_clock::now();
        std::string value = db->get(lastKey);
        const auto end = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Main: Son veri: " << lastKey << " -> " << value << "\n";
        std::cout << "Main: Son veri okuma suresi: " << duration.count() << " mikrosaniye\n";
    }

    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    size_t mem_bytes = db->approximate_memory_usage();
    double mem_mb = mem_bytes / 1024.0 / 1024.0;

    std::cout << "Main: Coroutine calismasi suresi: " << duration.count() << " mikrosaniye\n";
    std::cout << "Main: Coroutine calismasi suresi: " << duration.count() / 1000.0 << " ms\n";
    std::cout << "Main: Coroutine calismasi suresi: " << duration.count() / 1000000.0 << " s\n";
    std::printf("Main: Database veri boyutu: %.2f MB\n", mem_mb);
    // doing background run lol

    work.reset();

    thread.join();
    thread1.join();
    thread2.join();
    thread3.join();
    return 0;
}