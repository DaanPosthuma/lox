#pragma once

#include <functional>
#include <vector>

class Object;

class LoxCallable {
public:
    LoxCallable(std::function<Object(std::vector<Object> const&)> const& function, int arity);
    Object operator()(std::vector<Object> const& arguments) const;
    int arity() const { return mArity; };

private:
    std::function<Object(std::vector<Object> const&)> mFunction;
    int mArity;
};

inline bool operator ==(LoxCallable const& lhs, LoxCallable const& rhs) {
    return false;
}