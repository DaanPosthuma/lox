#include "Interpreter.h"
#include "Object.h"
#include "Expr.h"
#include "Dispatcher.h"

namespace {
    Object interpretBinaryExpr(BinaryExpr const& expr) {
        return {};
    }
    Object interpretGroupingExpr(GroupingExpr const& expr) {
        return {};
    }
    Object interpretLiteralExpr(LiteralExpr const& expr) {
        return expr.getValue();
    }
    Object interpretUnaryExpr(UnaryExpr const& expr) {
        return {};
    }
}

template <typename T>
using DispatcherFuncT = std::function<Object(T const&)>;

Object interpret(Expr const& expr) {
    static auto const dispatcher = Dispatcher<Object, Expr>(
        DispatcherFuncT<BinaryExpr>(interpretBinaryExpr),
        DispatcherFuncT<GroupingExpr>(interpretGroupingExpr),
        DispatcherFuncT<LiteralExpr>(interpretLiteralExpr),
        DispatcherFuncT<UnaryExpr>(interpretUnaryExpr)
    );

    return dispatcher.dispatch(expr);
}
