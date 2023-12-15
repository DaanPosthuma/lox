#include "LoxInstance.h"
#include "LoxClass.h"
#include <cassert>

LoxInstance::LoxInstance(LoxClass const& klass) : mClass(klass) {
}

LoxClass const& LoxInstance::klass() const { 
    return mClass;
}
