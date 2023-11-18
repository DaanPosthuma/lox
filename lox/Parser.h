#pragma once

#include <vector>

class Stmt;
class Token;

std::vector<Stmt*> parse(std::vector<Token> const& tokens);