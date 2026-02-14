#include <iostream>
#include <etcd/Client.hpp>
#include <boost/thread.hpp>
#include "./election/election.hpp"
#include <grpc/grpc.h>
#include <chrono>

using namespace std;
using namespace grpc_impl;

int main(int argc, char **argv)
{
    auto id = argv[1];
    auto _sleepMs = argv[2];
    etcd::Client client("http://127.0.0.1:2379");
    
    // Set gRPC timeout to avoid deadlock when waiting for lock
    client.set_grpc_timeout(std::chrono::seconds(10));

    int sleepMs = atoi(_sleepMs);
    std::cout << "id: " << id << ", sleepMs: " << sleepMs << std::endl;

    auto election = CElection(&client, id, sleepMs);
    election.thread->join();
    return 0;
}
