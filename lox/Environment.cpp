#include "Environment.h"
#include "Token.h"
#include "RuntimeError.h"

void Environment::define(std::string const& name, Object const& value) {
    mValues[name] = value;
}

Object Environment::get(Token const& name) const {
    if (auto it = mValues.find(name.getLexeme()); it != mValues.end()) {
        return it->second;
    }
    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}

void Environment::assign(Token const& name, Object const& value) {
    if (auto it = mValues.find(name.getLexeme()); it != mValues.end()) {
        it->second = value;
        return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}