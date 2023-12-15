#include "LoxCallable.h"
#include "Object.h"

LoxCallable::LoxCallable(FunctionType const& function, int arity, std::string const& name) : mFunction(function), mArity(arity), mName(name) {
}

Object LoxCallable::operator()(ArgsType arguments) const {
    return mFunction(arguments);
}
