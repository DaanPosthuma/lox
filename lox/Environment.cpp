#include "Environment.h"
#include "Token.h"
#include "RuntimeError.h"
#include <cassert>

Environment::Environment() : mEnclosing(nullptr) {}
Environment::Environment(Environment* environment) : mEnclosing(environment) {}

void Environment::define(std::string const& name, Object const& value) {
    mValues[name] = value;
}

Object Environment::get(Token const& name) const {
    if (auto it = mValues.find(name.lexeme()); it != mValues.end()) {
        return it->second;
    }
    if (mEnclosing) {
        return mEnclosing->get(name);
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme() + "'."};
}

Object Environment::getAt(int distance, std::string const& name) const {
    return ancestor(distance).mValues.at(name);
}

void Environment::assign(Token const& name, Object const& value) {
    if (auto it = mValues.find(name.lexeme()); it != mValues.end()) {
        it->second = value;
        return;
    }
    if (mEnclosing) {
        mEnclosing->assign(name, value);
        return;
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme() + "'."};
}

void Environment::assignAt(int distance, Token const& name, Object const& value) {
    ancestor(distance).mValues.at(name.lexeme()) = value;
}

Environment const& Environment::ancestor(int distance) const {
    auto environment = this;
    for (int i = 0; i != distance; ++i) {
        environment = environment->mEnclosing;
    }
    return *environment;
}

Environment& Environment::ancestor(int distance) {
    auto environment = this;
    for (int i = 0; i != distance; ++i) {
        environment = environment->mEnclosing;
    }
    return *environment;
}