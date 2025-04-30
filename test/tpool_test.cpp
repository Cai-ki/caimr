#include "thread/tpool.h"

#include <iostream>
#include <string>

#include "thread/cthread.h"

int main() {
    cai::tpool pool;
    pool.set_tsize(4);
    pool.start();

    for (int i = 0; i < 10; ++i) {
        pool.add([i] {
            std::cout << "task: " + std::to_string(i) +
                             " is executed by thread:" +
                             std::to_string(cai::cthread::tid())
                      << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待任务完成
    pool.stop();
    return 0;
}