#pragma once

#include "Object.h"
#include <vector>

class Stmt;
class Environment;

Object interpret(std::vector<Stmt*> const& statements, Environment& environment);
