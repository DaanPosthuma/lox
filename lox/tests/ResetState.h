#pragma once

#include "Lox.h"
#include "Environment.h"

class ResetState {
public:
    ~ResetState() {
        Lox::globals = Environment();
        Lox::locals.clear();
        Lox::hadError = false;
    }
};
