#pragma once

class Object;
class LoxInstance;
class LoxCallable;

#include <string>
#include <vector>
#include <unordered_map>

class LoxClass {
public:
    LoxClass(std::string const& name, std::unordered_map<std::string, LoxCallable> const& methods);
    std::string const& name() const { return mName; }
    int arity() const;
    LoxInstance operator()(std::vector<Object> const& arguments) const;
    Object findMethod(std::string const& name) const;

private:
    std::string mName;
    std::unordered_map<std::string, LoxCallable> mMethods;
};
