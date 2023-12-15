#pragma once

#include <functional>
#include <vector>
#include <string>

class Object;

class LoxCallable {
public:
    using ArgsType = std::vector<Object> const&;
    using ReturnType = Object;
    using FunctionType = std::function<ReturnType(ArgsType)>;
    LoxCallable(FunctionType const& function, int arity, std::string const& name);
    Object operator()(ArgsType arguments) const;
    int arity() const { return mArity; };
    std::string const& name() const { return mName; };

private:
    FunctionType mFunction;
    int mArity;
    std::string mName;
};

inline bool operator ==(LoxCallable const& lhs, LoxCallable const& rhs) {
    return false;
}
