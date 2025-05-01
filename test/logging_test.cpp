#include "log/logging.h"

int main() {
    LOG_DEBUG << "LOG_DEBUG: num=" << 1;
    LOG_INFO << "LOG_INFO: num=" << 2;
    LOG_ERROR << "LOG_ERROR: num=" << 3;
    LOG_DEBUGF("LOG_DEBUGF: num=%d, str=%s", 4, "4");
    LOG_INFOF("LOG_INFOF: num=%d, str=%s", 5, "5");
    LOG_ERRORF("LOG_ERRORF: num=%d, str=%s", 6, "6");
    LOG_FATALF("LOG_FATALF: num=%d, str=%s", 7, "7");
    return 0;
}