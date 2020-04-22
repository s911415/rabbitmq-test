#include "rabbit_mq.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    string line;
    auto con = open_rabbit();
    auto queue_name = declare_queue(con);

    while (std::getline(cin, line)) {
        send_message(con, line);
    }

    return 0;
}
