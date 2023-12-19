#pragma once

#include <unordered_map>

class Expr;
class Stmt;

void resolve(std::vector<Stmt const*> const& stmt);

