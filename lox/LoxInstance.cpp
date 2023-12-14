#include "LoxInstance.h"
#include "LoxClass.h"
#include <cassert>

LoxInstance::LoxInstance(std::string const& className) : mClassName(className) {
}

std::string const& LoxInstance::className() const { 
    return mClassName;
}
