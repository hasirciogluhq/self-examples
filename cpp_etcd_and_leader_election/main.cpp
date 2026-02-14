#include <iostream>
#include "etcd/Client.hpp"

using namespace std;

int main() {
    etcd::Client client("http://127.0.0.1:2379");

    // Key yaz
    client.put("/testkey", "merhaba").wait();

    // Key oku
    auto res = client.get("/testkey").get();
    std::cout << "Value: " << res.value() << std::endl;

    return 0;
}
