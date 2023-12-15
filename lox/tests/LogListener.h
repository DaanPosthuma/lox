#pragma once

class Object;
#include <vector>

class LogListener {
public:
    LogListener();
    std::vector<Object> const& history() const;
    ~LogListener();
private:
    std::vector<Object> mHistory;
};
