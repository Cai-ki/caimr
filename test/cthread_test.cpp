#include "caimr/thread/cthread.h"

#include <iostream>

int main() {
    cthread::cache();
    std::cout << "now thread id: " << cthread::tid() << std::endl;
    return 0;
}