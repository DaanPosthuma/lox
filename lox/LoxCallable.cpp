#include "LoxCallable.h"
#include "Object.h"
#include "Environment.h"

LoxCallable::LoxCallable(FunctionType const& function, int arity, std::string const& name)
    : mFunction([function](Environment*, ArgsType args) { return function(args); }), mClosure(nullptr), mArity(arity), mName(name) {
}
LoxCallable::LoxCallable(FunctionWithClosureType const& function, Environment* closure, int arity, std::string const& name) 
    : mFunction(function), mClosure(closure), mArity(arity), mName(name) {
}
Object LoxCallable::operator()(ArgsType arguments) const {
    return mFunction(mClosure, arguments);
}
LoxCallable LoxCallable::bind(LoxInstance const& instance) const {
    Environment* environment = new Environment(mClosure);
    environment->define("this", instance);
    return LoxCallable(mFunction, environment, mArity, mName);
}
