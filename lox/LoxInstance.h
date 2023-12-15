#pragma once
#include <string>
#include "LoxClass.h"

class LoxInstance {
public:
    LoxInstance(LoxClass const& klass);
    LoxClass const& klass() const;

private:
    LoxClass mClass;
};
