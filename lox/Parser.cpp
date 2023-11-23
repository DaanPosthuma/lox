#include "Parser.h"
#include "Token.h"
#include "TokenType.h"
#include "Expr.h"
#include "Stmt.h"
#include "Lox.h"

namespace {
    struct ParseError {
        Token token;
        std::string message;
    };

    class Parser {
    public:
        Parser(std::vector<Token> const& tokens) : mTokens(tokens) {}
        std::vector<Stmt*> parse();

    private:
        Stmt* declaration();
        Stmt* varDeclaration();
        Stmt* statement();
        Stmt* ifStatement();
        Stmt* printStatement();
        Stmt* whileStatement();
        Stmt* forStatement();
        Stmt* blockStatement();
        Stmt* expressionStatement();
        Expr* expression();
        Expr* assignment();
        Expr* oor ();
        Expr* aand ();
        Expr* equality();
        Expr* comparison();
        Expr* term();
        Expr* factor();
        Expr* unary();
        Expr* primary();

        void synchronise();

        Token const& advance();
        bool isAtEnd() const;
        Token const& peek() const;
        Token const& previous() const;

        template <TokenType T>
        bool check() const {
            if (isAtEnd()) return false;
            return peek().tokenType() == T;
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
            throw ParseError(peek(), message);
        }

        std::vector<Token> mTokens;
        int mCurrent = 0;
    };
}

std::vector<Stmt*> Parser::parse() {
    auto statements = std::vector<Stmt*>();
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

Stmt* Parser::declaration() {
    //try {
        if (match<TokenType::VAR>()) return varDeclaration();
        return statement();
    //}
    //catch (ParseError const& error) {
    //    synchronise();
    //    return nullptr;
    //}
}

Stmt* Parser::varDeclaration() {
    auto const& name = consume<TokenType::IDENTIFIER>("Expect variable name");
    auto const initializer = match<TokenType::EQUAL>() ? expression() : new LiteralExpr({});
    consume<TokenType::SEMICOLON>("Expect ';' after variable declaration.");
    return new VarStmt(name, initializer);
}

Stmt* Parser::statement() {
    if (match<TokenType::IF>()) return ifStatement();
    if (match<TokenType::WHILE>()) return whileStatement();
    if (match<TokenType::FOR>()) return forStatement();
    if (match<TokenType::PRINT>()) return printStatement();
    if (match<TokenType::LEFT_BRACE>()) return blockStatement();
    return expressionStatement();
}

Stmt* Parser::ifStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'if'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after 'if'.");
    auto const thenBranch = statement();
    auto const elseBranch = match<TokenType::ELSE>() ? statement() : nullptr;
    return new IfStmt(condition, thenBranch, elseBranch);
}

Stmt* Parser::printStatement() {
    auto const value = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after value.");
    return new PrintStmt(value);
}

Stmt* Parser::whileStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'while'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after condition.");
    auto const body = statement();
    return new WhileStmt(condition, body);
}

Stmt* Parser::forStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'for'.");
    
    Stmt* initializer;
    if (match<TokenType::SEMICOLON>()) {
        initializer = nullptr;
    }
    else if (match<TokenType::VAR>()) {
        initializer = varDeclaration();
    }
    else {
        initializer = expressionStatement();
    }

    auto const condition = check<TokenType::SEMICOLON>() ? new LiteralExpr(true) : expression();
    consume<TokenType::SEMICOLON>("Expect ';' after condition.");

    auto const increment = check<TokenType::RIGHT_PAREN>() ? nullptr: expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after for clause.");

    auto body = statement();

    if (increment) {
        body = new BlockStmt({ body, new ExpressionStmt(increment) });
    }

    body = new WhileStmt(condition, body);
    
    if (initializer) {
        body = new BlockStmt({ initializer, body });
    }

    return body;
}

Stmt* Parser::blockStatement() {
    auto statements = std::vector<Stmt*>();

    while (!check<TokenType::RIGHT_BRACE>() && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume<TokenType::RIGHT_BRACE>("Expect '}' after block.");
    return new BlockStmt(statements);
}

Stmt* Parser::expressionStatement() {
    auto const expr = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after expression.");
    return new ExpressionStmt(expr);
}

Expr* Parser::expression() {
    return assignment();
}

Expr* Parser::assignment() {
    auto const expr = oor();

    if (match<TokenType::EQUAL>()) {
        auto const equals = previous();
        auto const value = assignment();

        if (auto const variableExpr = dynamic_cast<VariableExpr*>(expr)) {
            auto const name = variableExpr->name();
            return new AssignExpr(name, value);
        }

        throw ParseError(equals, "Invalid assignment target.");
    }

    return expr;
}

Expr* Parser::oor() {
    auto expr = aand();

    while (match<TokenType::OR>()) {
        auto const operatr = previous();
        auto const right = aand();
        expr = new LogicalExpr(expr, operatr, right);
    }

    return expr;
}
Expr* Parser::aand() {
    auto expr = equality();

    while (match<TokenType::AND>()) {
        auto const operatr = previous();
        auto const right = equality();
        expr = new LogicalExpr(expr, operatr, right);
    }

    return expr;
}

Expr* Parser::equality() {
    auto expr = comparison();
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
        return new LiteralExpr(previous().literal());
    }

    if (match<TokenType::IDENTIFIER>()) {
        return new VariableExpr(previous());
    }

    if (match<TokenType::LEFT_PAREN>()) {
        auto const expr = expression();
        consume<TokenType::RIGHT_PAREN>("Expect ')' after expression.");
        return new GroupingExpr(expr);
    }

    throw ParseError(peek(), "Expect expression.");
}

void Parser::synchronise() {
    advance();
    while (!isAtEnd()) {
        if (previous().tokenType() == TokenType::SEMICOLON) return;
        
        switch (peek().tokenType()) {
        case TokenType::CLASS:
        case TokenType::FOR:
        case TokenType::FUN:
        case TokenType::IF:
        case TokenType::PRINT:
        case TokenType::RETURN:
        case TokenType::VAR:
        case TokenType::WHILE:
            return;
        }

        advance();
    }
}

Token const& Parser::advance() {
    if (!isAtEnd()) mCurrent++;
    return previous();
}

bool Parser::isAtEnd() const{
    return peek().tokenType() == TokenType::END_OF_FILE;
}

Token const& Parser::peek() const {
    return mTokens.at(mCurrent);
}

Token const& Parser::previous() const {
    return mTokens.at(mCurrent - 1);
}

std::vector<Stmt*> parse(std::vector<Token> const& tokens) {
    try {
        auto parser = Parser(tokens);
        return parser.parse();
    }
    catch (ParseError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}
