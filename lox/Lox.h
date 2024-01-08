#pragma once

#include "Resolver.h"
#include "Expr.h"
#include <string>
#include <cassert>

class Environment;
class Token;

struct ExprHash {
    auto operator()(xyz::polymorphic<Expr> const& expr) const -> std::size_t {
        return 0;
    }
};

struct ExprEq {
    auto operator()(
        xyz::polymorphic<Expr> const& lhs,
        xyz::polymorphic<Expr> const& rhs
        ) const -> bool {
        return lhs->IsEqual(*rhs);
    }
};

using ResolvedLocals = std::unordered_map<xyz::polymorphic<Expr>, int, ExprHash, ExprEq>;

class Lox {
public:
    static void error(int line, std::string message);
    static void report(int line, std::string where, std::string message);
    static void error(Token const& token, std::string const& message);

    static bool hadError;
    static bool debugEnabled;
    static Environment globals;
    static ResolvedLocals locals;
};
