#include "Environment.h"
#include "Token.h"
#include "RuntimeError.h"
#include <cassert>

Environment::Environment() : mEnclosing(nullptr) {}

Environment::Environment(Environment& environment) : mEnclosing(&environment) {}

void Environment::define(std::string const& name, Object const& value) {
    mValues[name] = value;
}

Object Environment::get(Token const& name) const {
    if (auto it = mValues.find(name.getLexeme()); it != mValues.end()) {
        return it->second;
    }
    if (mEnclosing) {
        return mEnclosing->get(name);
    }
    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}

void Environment::assign(Token const& name, Object const& value) {
    if (auto it = mValues.find(name.getLexeme()); it != mValues.end()) {
        it->second = value;
        return;
    }
    if (mEnclosing) {
        mEnclosing->assign(name, value);
        return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}