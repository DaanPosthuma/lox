#pragma once

#include "Object.h"
#include <unordered_map>

class Token;

class Environment {
public:
    void define(std::string const& name, Object const& value);
    Object get(Token const& name) const;
    void assign(Token const& name, Object const& value);

private:
    std::unordered_map<std::string, Object> mValues;
};
