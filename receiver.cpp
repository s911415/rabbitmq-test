#include <iostream>
#include <string>
#include "rabbit_mq.h"

using namespace std;

int main() {
    string line;
    auto con = open_rabbit();
    auto queue_name = declare_queue(con);

    receive_message(con, queue_name, [](const string &message) {
        cout << message << endl;
    });

    return 0;
}
