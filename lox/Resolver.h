#pragma once

#include "polymorphic.h"
#include <vector>

class Expr;
class Stmt;

void resolve(std::vector<xyz::polymorphic<Stmt>> const& stmt);

