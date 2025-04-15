#include "caimr/thread/cthread.h"

#include <iostream>

int main() {
    cthread::cacheTid();
    std::cout << "now thread id: " << cthread::tid() << std::endl;
    return 0;
}