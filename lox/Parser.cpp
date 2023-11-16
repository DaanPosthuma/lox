#include "Parser.h"
#include "Token.h"
#include "TokenType.h"
#include "Expr.h"
#include "Lox.h"

class ParseError {};

Expr* Parser::parse() {
    try {
        return expression();
    }
    catch (ParseError const&) {
        return nullptr;
    }
}

Expr* Parser::expression() {
    return equality();
}

Expr* Parser::equality() {
    Expr* expr = comparison();
    while (match<TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL>()) {
        auto const operatr = previous();
        auto const right = comparison();
        expr = new BinaryExpr(expr, operatr, right);
    }

    return expr;
}

Expr* Parser::comparison() {
    auto expr = term();

    while (match<TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL>()) {
        auto const operatr = previous();
        auto const right = term();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr* Parser::term() {
    auto expr = factor();

    while (match<TokenType::MINUS, TokenType::PLUS>()) {
        auto const operatr = previous();
        auto const right = factor();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr* Parser::factor() {
    auto expr = unary();

    while (match<TokenType::SLASH, TokenType::STAR>()) {
        auto const operatr = previous();
        auto const right = unary();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr* Parser::unary() {
    if (match<TokenType::BANG, TokenType::MINUS>()) {
        auto const operatr = previous();
        auto const right = unary();
        return new UnaryExpr(operatr, right);
    }

    return primary();
}

Expr* Parser::primary() {
    if (match<TokenType::FALSE>()) return new LiteralExpr(false);
    if (match<TokenType::TRUE>()) return new LiteralExpr(true);
    if (match<TokenType::NIL>()) return new LiteralExpr({});
    
    if (match<TokenType::NUMBER, TokenType::STRING>()) {
        return new LiteralExpr(previous().getLiteral());
    }

    if (match<TokenType::LEFT_PAREN>()) {
        auto const expr = expression();
        consume<TokenType::RIGHT_PAREN>("Expect ')' after expression.");
        return new GroupingExpr(expr);
    }

    throw error(peek(), "Expect expression.");
}

Token const& Parser::advance() {
    if (!isAtEnd()) mCurrent++;
    return previous();
}

bool Parser::isAtEnd() const{
    return peek().getTokenType() == TokenType::END_OF_FILE;
}

Token const& Parser::peek() const {
    return mTokens.at(mCurrent);
}

Token const& Parser::previous() const {
    return mTokens.at(mCurrent - 1);
}

ParseError Parser::error(Token const& token, std::string const& message) const {
    Lox::error(token, message);
    return ParseError();
}
