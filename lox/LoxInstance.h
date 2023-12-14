#pragma once
#include <string>
class LoxClass;

class LoxInstance {
public:
    LoxInstance(std::string const& className);
    std::string const& className() const;

private:
    std::string mClassName;
};
