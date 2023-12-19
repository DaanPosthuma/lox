#pragma once

#include <functional>
#include <vector>
#include <string>

class Object;
class Environment;
class LoxInstance;

class LoxCallable {
public:
    using ArgsType = std::vector<Object> const&;
    using ReturnType = Object;
    using FunctionType = std::function<ReturnType(ArgsType)>;
    using FunctionWithClosureType = std::function<ReturnType(Environment*, ArgsType)>;
    LoxCallable(FunctionType const& function, int arity, std::string const& name);
    LoxCallable(FunctionWithClosureType const& function, Environment* closure, int arity, std::string const& name);
    Object operator()(ArgsType arguments) const;
    int arity() const { return mArity; };
    std::string const& name() const { return mName; };
    LoxCallable bind(LoxInstance const& instance) const;
private:
    
    LoxCallable::FunctionWithClosureType mFunction;
    Environment* mClosure;
    int mArity;
    std::string mName;
};

inline bool operator ==(LoxCallable const& lhs, LoxCallable const& rhs) {
    return false;
}
