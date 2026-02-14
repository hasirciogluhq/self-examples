#pragma once
#include <etcd/Client.hpp>
#include <boost/thread.hpp>

class CElection
{
public:
    CElection(etcd::Client *client, std::string id, int sleepMs);
    boost::thread *thread;
    int sleepMs;
    std::string id;
    bool leader = false;
    bool running = true;
    std::string lockId = ""; // Store lock ID to keep it alive

    // functions
    const char *info();
    void claimLeader();
    bool hearthBeat();
    void stop();

private:
    etcd::Client *client;
    void threadRunner();
};