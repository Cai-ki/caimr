#include "caimr/common/time.h"

#include <iostream>

int main() {
    cai::time::time_point tp = cai::time::clock::now();
    std::cout << "now time: " << cai::time::to_string(tp) << std::endl;
    return 0;
}