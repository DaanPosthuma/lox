#include "LoxCallable.h"
#include "Object.h"

LoxCallable::LoxCallable(std::function<Object(std::vector<Object> const&)> const& function, int arity) : mFunction(function), mArity(arity) {
}

Object LoxCallable::operator()(std::vector<Object> const& arguments) const {
    return mFunction(arguments);
}
