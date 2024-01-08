#pragma once

#include "Object.h"
#include "Resolver.h"
#include <vector>

class Stmt;
class Environment;

Object interpret(std::vector<xyz::polymorphic<Stmt>> const& statements);
