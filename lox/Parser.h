#pragma once

#include "polymorphic.h"
#include <vector>

class Stmt;
class Token;

std::vector<xyz::polymorphic<Stmt>> parse(std::vector<Token> const& tokens);