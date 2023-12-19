#include "LoxClass.h"
#include "LoxClass.h"
#include "LoxClass.h"
#include "Object.h"
#include "LoxCallable.h"
#include <cassert>

LoxClass::LoxClass(std::string const& name, std::unordered_map<std::string, LoxCallable> const& methods) : mName(name), mMethods(methods) {
}

int LoxClass::arity() const { 
    auto const initializer = findMethod("init");
    if (initializer.isLoxCallable()) {
        return static_cast<LoxCallable>(initializer).arity();
    }
    return 0;
}

LoxInstance LoxClass::operator()(std::vector<Object> const& arguments) const {
    auto const instance = LoxInstance(*this);
    auto const initializer = findMethod("init");
    if (initializer.isLoxCallable()) {
        static_cast<LoxCallable>(initializer).bind(instance)(arguments);
    }
    return instance;
}

Object LoxClass::findMethod(std::string const& name) const {
    return mMethods.contains(name) ? mMethods.at(name) : Object();
}
