#include "caimr/thread/cthread.h"

#include <iostream>

int main() {
    cai::cthread::cache();
    std::cout << "now thread id: " << cai::cthread::tid() << std::endl;
    return 0;
}