#pragma once

#include <iostream>
#include <iostream>
#include <string>
#include <mutex>
#include <unordered_map>

class SDatabase
{
public:
    static SDatabase *getInstance()
    {
        static SDatabase *instance = nullptr;
        if (!instance)
        {
            instance = new SDatabase();
        }

        return instance;
    }

    std::unordered_map<std::string, std::string> data = {};
    std::string get(const std::string &key) const
    {
        // lock it
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);
        auto it = data.find(key);
        if (it != data.end())
            return it->second;
        return "";
    }

    bool set(const std::string &key, const std::string &value)
    {
        // lock it
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);
        return data.emplace(key, value).second;
    }

    size_t approximate_memory_usage()
    {
        size_t size = sizeof(data); // header
        for (auto &[k, v] : data)
        {
            size += sizeof(std::pair<const std::string, std::string>);
            size += k.capacity(); // string buffer
            size += v.capacity();
        }
        return size;
    }

private:
    SDatabase() = default;
    ~SDatabase() = default;

    SDatabase(const SDatabase &) = delete;
    SDatabase &operator=(const SDatabase &) = delete;

    static SDatabase *instance;
    static std::mutex mtx;
};