#include "caimr/common/timestamp.h"

#include <iostream>

int main() {
    cai::timestamp now = cai::timestamp::now();
    std::cout << "now time: " << now.to_string() << std::endl;
    return 0;
}