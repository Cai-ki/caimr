#include "caimr/thread/thread.h"

#include <iostream>

#include "caimr/thread/cthread.h"

void task() {
    std::cout << "thread: " << cai::cthread::tid() << " is running"
              << std::endl;
}

int main() {
    cai::thread thread(task, "test");
    thread.start();
    thread.join();
    std::cout << "thread name: " << thread.name() << std::endl;
    return 0;
}