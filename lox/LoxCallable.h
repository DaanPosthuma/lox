#pragma once

#include <functional>
#include <vector>
#include <string>

class Object;

class LoxCallable {
public:
    LoxCallable(std::function<Object(std::vector<Object> const&)> const& function, int arity, std::string const& name);
    Object operator()(std::vector<Object> const& arguments) const;
    int arity() const { return mArity; };
    std::string const& name() const { return mName; };

private:
    std::function<Object(std::vector<Object> const&)> mFunction;
    int mArity;
    std::string mName;
};

inline bool operator ==(LoxCallable const& lhs, LoxCallable const& rhs) {
    return false;
}
