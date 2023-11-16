#pragma once

#include <vector>
#include <exception>
#include <string>

class Token;
enum class TokenType;
class Expr;
class ParseError;

class Parser {
public:
    Parser(std::vector<Token> const& tokens) : mTokens(tokens) {}
    Expr* parse();

private:
    Expr* expression();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* primary();


    Token const& advance();
    bool isAtEnd() const;
    Token const& peek() const;
    Token const& previous() const;

    template <TokenType T>
    bool check() {
        if (isAtEnd()) return false;
        return peek().getTokenType() == T;
    }

    template <TokenType T>
    bool match() { 
        if (check<T>()) {
            advance();
            return true;
        }
        return false;
    }

    template <TokenType First, TokenType Second, TokenType... Rest>
    bool match() { 
        return match<First>() || match<Second, Rest...>();
    }

    template <TokenType T>
    Token const& consume(std::string const& message) {
        if (check<T>()) return advance();
        throw error(peek(), message);
    }

    ParseError error(Token const& token, std::string const& message) const;

    std::vector<Token> mTokens;
    int mCurrent = 0;
};
