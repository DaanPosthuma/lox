#include <string>
#include "ExprToString.h"
#include "Expr.h"
#include "Dispatcher.h"

namespace {

    std::string binaryExprToString(BinaryExpr const& expr) {
        return "(" + expr.getOperator().getLexeme() + " " + toString(expr.getLeft()) + " " + toString(expr.getRight()) + ")";
    }

    std::string groupingExprToString(GroupingExpr const& expr) {
        return "(group " + toString(expr.getExpression()) + ")";
    }

    std::string literalExprToString(LiteralExpr const& expr) {
        return expr.getValue().toString();
    }

    std::string unaryExprToString(UnaryExpr const& expr) {
        return "(" + expr.getOperator().getLexeme() + " " + toString(expr.getRight()) + ")";
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
