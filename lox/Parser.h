#pragma once

#include <vector>

class Stmt;
class Token;

std::vector<Stmt const*> parse(std::vector<Token> const& tokens);