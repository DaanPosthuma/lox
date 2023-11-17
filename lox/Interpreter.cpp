#include "Interpreter.h"
#include "Object.h"
#include "Expr.h"
#include "Dispatcher.h"
#include "TokenType.h"
#include "Lox.h"
#include <stdexcept>

namespace {

    struct RuntimeError {
        Token token;
        std::string message;
    };

    bool isTruthy(Object const& object) {
        if (object.isNil()) return false;
        if (object.isBoolean()) return (bool)object;
        return true;
    }

    void checkNumberOperand(Token const& token, Object const& object) {
        if (!object.isDouble()) throw RuntimeError(token, "Operand must be a number.");
    }

    void checkNumberOperands(Token const& token, Object const& left, Object const& right) {
        if (!left.isDouble() || !right.isDouble()) throw RuntimeError(token, "Operands must be numbers.");
    }

    Object evaluateBinaryExpr(BinaryExpr const& expr) {
        auto const left = interpret(expr.getLeft());
        auto const right = interpret(expr.getRight());
        auto const& operatr = expr.getOperator();

        switch (operatr.getTokenType()) {
        case TokenType::MINUS:
            checkNumberOperands(operatr, left, right);
            return (double)left - (double)right;
        case TokenType::PLUS:
            
            if (left.isString() || right.isString())
                return left.toString() + right.toString();

            if (left.isDouble() && right.isDouble())
                return (double)left + (double)right;

            throw RuntimeError(operatr, "Cannot concatenate " + left.toString() + " and " + right.toString() + ".");
        case TokenType::SLASH:
            checkNumberOperands(operatr, left, right);
            return (double)left / (double)right;
        case TokenType::STAR:
            checkNumberOperands(operatr, left, right);
            return (double)left * (double)right;
        case TokenType::BANG_EQUAL:
            return left != right;
        case TokenType::EQUAL_EQUAL:
            return left == right;
        case TokenType::GREATER:
            checkNumberOperands(operatr, left, right);
            return (double)left > (double)right;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(operatr, left, right);
            return (double)left >= (double)right;
        case TokenType::LESS:
            checkNumberOperands(operatr, left, right);
            return (double)left < (double)right;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(operatr, left, right);
            return (double)left <= (double)right;
        }

        throw RuntimeError(expr.getOperator(), "Sorry I cannot do this!");
    }
    Object evaluateGroupingExpr(GroupingExpr const& expr) {
        return interpret(expr.getExpression());
    }
    Object evaluateLiteralExpr(LiteralExpr const& expr) {
        return expr.getValue();
    }
    Object evaluateUnaryExpr(UnaryExpr const& expr) {
        auto const right = interpret(expr.getRight());
        auto const& operatr = expr.getOperator();

        switch (operatr.getTokenType()) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            checkNumberOperand(operatr, right);
            return -(double)right;
            break;
        default:
            throw std::logic_error("what happen?");
        }
    }
}

template <typename T>
using DispatcherFuncT = std::function<Object(T const&)>;

Object interpret(Expr const& expr) {
    static auto const evaluateDispatcher = Dispatcher<Object, Expr>(
        DispatcherFuncT<BinaryExpr>(evaluateBinaryExpr),
        DispatcherFuncT<GroupingExpr>(evaluateGroupingExpr),
        DispatcherFuncT<LiteralExpr>(evaluateLiteralExpr),
        DispatcherFuncT<UnaryExpr>(evaluateUnaryExpr)
    );

    try {
        return evaluateDispatcher.dispatch(expr);
    }
    catch (RuntimeError const& error) {
        Lox::error(error.token, error.message);
    }
    return {};
}
