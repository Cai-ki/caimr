#include "caimr/logger/logstream.h"

#include <assert.h>

#include <iostream>

int main() {
    logstream stream;
    stream << "pi: " << 3.1415926535 << ", count: " << 1234567890 << '!';
    assert(stream.buffer().to_string() ==
           "pi: 3.1415926535, count: 1234567890!");

    std::cout << stream.buffer().to_string() << std::endl;
    return 0;
}