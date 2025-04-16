#include "caimr/logger/logging.h"

int main() {
    LOG_DEBUG << "LOG_DEBUG: " << 1;
    LOG_INFO << "LOG_INFO: " << 2;
    LOG_ERROR << "LOG_ERROR: " << 3;
    LOG_FATAL << "LOG_FATAL: " << 4;
    return 0;
}