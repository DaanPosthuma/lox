#pragma once

#include "Resolver.h"
#include "Expr.h"
#include <string>
#include <cassert>

class Environment;
class Token;

struct ExprHash {
    auto operator()(Expr const* expr) const -> std::size_t {
        return 0;
    }
};

struct ExprEq {
    auto operator()(
        Expr const* lhs,
        Expr const* rhs
        ) const -> bool {
        auto const ptrsEq = lhs == rhs;
        auto const logicalEq = lhs->IsEqual(*rhs);
        if (ptrsEq) assert(logicalEq);
        if (!ptrsEq && logicalEq) {
            int test = 3;
        }
        return logicalEq;
    }
};

using ResolvedLocals = std::unordered_map<Expr const*, int, ExprHash, ExprEq>;

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
