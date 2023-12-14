#pragma once

class Object;
class LoxInstance;
#include <string>
#include <vector>

class LoxClass {
public:
    LoxClass(std::string const& name);
    std::string const& name() const { return mName; }
    int arity() const { return 0; }
    LoxInstance operator()(std::vector<Object> const& arguments) const;

private:
    std::string mName;
};
