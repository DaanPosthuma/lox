#include <string>
#include "ExprToString.h"
#include "Expr.h"
#include "Dispatcher.h"


template <typename T>
using DispatcherFuncT = std::function<std::string(T const&)>;

std::string toString(Expr const& expr) {

    static auto const dispatcher = Dispatcher<std::string, Expr>(
        static_cast<DispatcherFuncT<BinaryExpr>>([](BinaryExpr const& expr) { return toString(expr); }),
        static_cast<DispatcherFuncT<GroupingExpr>>([](GroupingExpr const& expr) { return toString(expr); }),
        static_cast<DispatcherFuncT<LiteralExpr>>([](LiteralExpr const& expr) { return toString(expr); }),
        static_cast<DispatcherFuncT<UnaryExpr>>([](UnaryExpr const& expr) { return toString(expr); })
    );

    return dispatcher.dispatch(expr);
}

std::string toString(BinaryExpr const& expr) {
    return "(" + expr.getOperator().getLexeme() + " " + toString(expr.getLeft()) + " " + toString(expr.getRight()) + ")";
}

std::string toString(GroupingExpr const& expr) {
    return "(group " + toString(expr.getExpression()) + ")";
}

std::string toString(LiteralExpr const& expr) {
    return expr.getValue().toString();
}

std::string toString(UnaryExpr const& expr) {
    return "(" + expr.getOperator().getLexeme() + " " + toString(expr.getRight()) + ")";
}
