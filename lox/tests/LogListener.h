#pragma once

#include "Lox.h"
#include "Environment.h"
#include <vector>

class LogListener {
public:
    LogListener() {
        Lox::globals.define("log", LoxCallable([&](LoxCallable::ArgsType args) -> Object {
            mHistory.emplace_back(args[0]);
            return {};
            }, 1, "log"));
    }

    std::vector<Object> const& history() const { return mHistory; }

    ~LogListener() {
        Lox::globals.remove("log");
    }
private:
    std::vector<Object> mHistory;
};
