#include "LogListener.h"
#include "Object.h"
#include "Lox.h"
#include "Environment.h"

LogListener::LogListener() {
    Lox::globals.define("log", LoxCallable([&](LoxCallable::ArgsType args) -> Object {
        mHistory.emplace_back(args[0]);
        return {};
        }, 1, "log"));
}

std::vector<Object> const& LogListener::history() const { return mHistory; }

LogListener::~LogListener() {
    Lox::globals.remove("log");
}
