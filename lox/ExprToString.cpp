#include <string>
#include "ExprToString.h"
#include "Expr.h"
#include "Dispatcher.h"

namespace {

    std::string binaryExprToString(BinaryExpr const& expr) {
        return "(" + expr.operatr().lexeme() + " " + toString(expr.left()) + " " + toString(expr.right()) + ")";
    }

    std::string groupingExprToString(GroupingExpr const& expr) {
        return "(group " + toString(expr.expression()) + ")";
    }

    std::string literalExprToString(LiteralExpr const& expr) {
        return expr.value().toString();
    }

    std::string unaryExprToString(UnaryExpr const& expr) {
        return "(" + expr.operatr().lexeme() + " " + toString(expr.right()) + ")";
    }

}

template <typename T>
using DispatcherFuncT = std::function<std::string(T const&)>;

std::string toString(Expr const& expr) {

    static auto const dispatcher = Dispatcher<std::string, Expr const&>(
        DispatcherFuncT<BinaryExpr>(binaryExprToString),
        DispatcherFuncT<GroupingExpr>(groupingExprToString),
        DispatcherFuncT<LiteralExpr>(literalExprToString),
        DispatcherFuncT<UnaryExpr>(unaryExprToString)
    );

    return dispatcher.dispatch(expr);
}
