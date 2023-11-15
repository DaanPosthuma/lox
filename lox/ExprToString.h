#pragma once

#include <string>

class Expr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

std::string toString(Expr const& expr);
std::string toString(BinaryExpr const& expr);
std::string toString(GroupingExpr const& expr);
std::string toString(LiteralExpr const& expr);
std::string toString(UnaryExpr const& expr);
