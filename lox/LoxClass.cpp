#include "LoxClass.h"
#include "LoxClass.h"
#include "LoxClass.h"
#include "Object.h"
#include "LoxCallable.h"
class LoxFunction;

LoxClass::LoxClass(std::string const& name, std::unordered_map<std::string, LoxCallable> const& methods) : mName(name), mMethods(methods) {
}

LoxInstance LoxClass::operator()(std::vector<Object> const& arguments) const {
    return LoxInstance(*this);
}

Object LoxClass::findMethod(std::string const& name) const {
    return mMethods.contains(name) ? mMethods.at(name) : Object();
}
