#include "LoxCallable.h"
#include "Object.h"

LoxCallable::LoxCallable(std::function<Object(std::vector<Object> const&)> const& function, int arity, std::string const& name) : mFunction(function), mArity(arity), mName(name) {
}

Object LoxCallable::operator()(std::vector<Object> const& arguments) const {
    return mFunction(arguments);
}
