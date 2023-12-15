#pragma once

#include "Object.h"
#include <unordered_map>

class Token;

class Environment {
public:
    Environment();
    Environment(Environment const&) = delete;
    Environment(Environment* environment);

    void define(std::string const& name, Object const& value);
    Object get(Token const& name) const;
    Object getAt(int distance, std::string const& name) const;
    void assign(Token const& name, Object const& value);
    void assignAt(int distance, Token const& name, Object const& value);
    void remove(std::string const& name);

private:
    Environment const& ancestor(int distance) const;
    Environment& ancestor(int distance);

    std::unordered_map<std::string, Object> mValues;
    Environment* mEnclosing;
};
