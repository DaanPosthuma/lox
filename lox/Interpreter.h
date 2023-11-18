#pragma once

#include <vector>

class Stmt;
class Environment;

void interpret(std::vector<Stmt*> const& statements, Environment& environment);
