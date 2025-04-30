#include "logger/fixedbuffer.h"

#include <iostream>

int main() {
    cai::fixedbuffer<cai::KB_BUFFER_SIZE> buf;
    buf.append("hello", 5);

    std::cout << "buffer capacity: " << buf.cap() << " size: " << buf.size()
              << " used: " << buf.used() << std::endl;
    return 0;
}