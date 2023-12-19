#pragma once

#include "Object.h"
#include "Resolver.h"
#include <vector>

class Stmt;
class Environment;

Object interpret(std::vector<Stmt const*> const& statements);
