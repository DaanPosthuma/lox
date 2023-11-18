#pragma once

#include "Token.h"
#include <string>

struct RuntimeError {
    Token token;
    std::string message;
};
