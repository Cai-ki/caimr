#include "caimr/core/timestamp.h"

#include <iostream>

int main() {
    timestamp now = timestamp::now();
    std::cout << "now time: " << now.to_string() << std::endl;
    return 0;
}