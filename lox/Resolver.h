#pragma once

#include <unordered_map>

class Expr;
class Stmt;

using ResolvedLocals = std::unordered_map<Expr const*, int>;

ResolvedLocals resolve(std::vector<Stmt const*> const& stmt);

