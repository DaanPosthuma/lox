#include "LoxClass.h"
#include "Object.h"
#include <vector>

LoxClass::LoxClass(std::string const& name) : mName(name) {
}

LoxInstance LoxClass::operator()(std::vector<Object> const& arguments) const {
    return LoxInstance(mName);
}
