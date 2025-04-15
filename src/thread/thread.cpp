#include "caimr/thread/thread.h"

#include <semaphore.h>

#include "caimr/thread/cthread.h"

std::atomic_int thread::tcount_(0);

thread::thread(tfunc func, const std::string& name)
    : started_(false),
      joined_(false),
      tid_(0),
      func_(std::move(func)),
      name_(name) {
    set_default();
}

thread::~thread() {
    if (started_ && !joined_) thread_->detach();
}

void thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    thread_ = std::make_shared<std::thread>([&]() {
        tid_ = cthread::tid();
        sem_post(&sem);
        func_();
    });
    sem_wait(&sem);
}

void thread::join() {
    joined_ = true;
    thread_->join();
}

void thread::set_default() {
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "thread%d", ++tcount_);
        name_ = buf;
    }
}