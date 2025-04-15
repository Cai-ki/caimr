#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "caimr/core/noncopyable.h"

class thread : noncopyable {
   public:
    using tfunc = std::function<void()>;
    explicit thread(tfunc func, const std::string& name = {});
    ~thread();

    void start();
    void join();
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

   private:
    void set_default();
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    tfunc func_;
    std::string name_;
    static std::atomic_int tcount_;
};