#pragma once

#include <vector>

class Expr;
class Token;

Expr* parse(std::vector<Token> const& tokens);