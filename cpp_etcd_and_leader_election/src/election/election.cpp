#include "election.hpp"
#include <boost/thread/thread.hpp>

CElection::CElection(etcd::Client *client, std::string id, int sleepMs)
{
    this->client = client;
    this->id = id;
    this->sleepMs = sleepMs;

    this->thread = new boost::thread(&CElection::threadRunner, this);
    // Don't detach - join properly for clean shutdown
}

void CElection::threadRunner()
{
    while (running)
    {
        claimLeader();

        boost::this_thread::sleep(boost::posix_time::milliseconds(sleepMs));
    }
}

void CElection::claimLeader()
{
    if (leader && !lockId.empty())
    {
        // Already leader - refresh heartbeat (keep TTL alive)
        hearthBeat();
        return;
    }

    // Try to get lock - keep trying until we get it
    try
    {
        auto lock_res = client->lock("mama", 10).get();
        if (lock_res.is_ok())
        {
            lockId = lock_res.value().as_string();
            std::cout << id << " I am the leader!" << std::endl;
            std::cout.flush();
            leader = true;
        }
        else
        {
            std::cout << id << " NOT OK!" << ", leader: " << lock_res.value() << ", err: " << lock_res.error_message() << ", errCode: " << lock_res.error_code() << std::endl;
        }
        // If not ok, just continue trying next time - don't give up
        return;
    }
    catch (const std::exception &e)
    {
        // Lock attempt timed out or failed, will retry next cycle
        // Don't spam errors, just continue
        std::cout << id << " CERROR!" << std::endl;
        return;
    }
    catch (...)
    {
        std::cout << id << " CECCCCCCRROR!" << std::endl;
        // Unknown error, will retry
        return;
    }
}

bool CElection::hearthBeat()
{
    // Refresh lock - keep it alive
    if (lockId.empty())
        return false;

    try
    {
        // Keep lock alive by renewing it
        auto renew_res = client->get("mama").get();
        return renew_res.is_ok();
    }
    catch (...)
    {
        std::cout << id << " HB ERR!" << std::endl;
        return false;
    }
};

const char *CElection::info()
{
    // Example logic for sending heartbeat
    return "info text";
};

void CElection::stop()
{
    running = false;
    if (thread && thread->joinable())
    {
        thread->join();
    }
};