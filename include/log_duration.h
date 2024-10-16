#pragma once

#include <chrono>
#include <iostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X ## Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(X) LogDuration UNIQUE_VAR_NAME_PROFILE(X)

class LogDuration {
public:
  using Clock = std::chrono::steady_clock;

  LogDuration(std::string task_name) : task_name_(task_name) {}

  ~LogDuration() {
    using namespace std::chrono;
    using namespace std::literals;

    const auto end_time = Clock::now();
    const auto dur = end_time - start_time_;
    std::cerr << task_name_ <<  ": "s << duration_cast<milliseconds>(dur).count() << " ms"s
              << std::endl;
  }

private:
  const Clock::time_point start_time_ = Clock::now();
  const std::string task_name_;
};
