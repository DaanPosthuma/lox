#include "Parser.h"
#include "Token.h"
#include "TokenType.h"
#include "Expr.h"
#include "Stmt.h"
#include "Lox.h"
#include "polymorphic.h"

namespace {
    struct ParseError {
        Token token;
        std::string message;
    };

    class Parser {
    public:
        Parser(std::vector<Token> const& tokens) : mTokens(tokens) {}
        std::vector<xyz::polymorphic<Stmt>> parse();

    private:
        xyz::polymorphic<Stmt> declaration();
        ClassStmt classDeclaration();
        VarStmt varDeclaration();
        xyz::polymorphic<Stmt> statement();
        IfStmt ifStatement();
        PrintStmt printStatement();
        WhileStmt whileStatement();
        ReturnStmt returnStatement();
        xyz::polymorphic<Stmt> forStatement();
        BlockStmt blockStatement();
        ExpressionStmt expressionStatement();
        FunctionStmt function(std::string const& kind);
        xyz::polymorphic<Expr> expression();
        xyz::polymorphic<Expr> assignment();
        xyz::polymorphic<Expr> oor ();
        xyz::polymorphic<Expr> aand ();
        xyz::polymorphic<Expr> equality();
        xyz::polymorphic<Expr> comparison();
        xyz::polymorphic<Expr> term();
        xyz::polymorphic<Expr> factor();
        xyz::polymorphic<Expr> unary();
        xyz::polymorphic<Expr> call();
        xyz::polymorphic<Expr> finishCall(xyz::polymorphic<Expr> callee);
        xyz::polymorphic<Expr> primary();

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

std::vector<xyz::polymorphic<Stmt>> Parser::parse() {
    auto statements = std::vector<xyz::polymorphic<Stmt>>();
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

xyz::polymorphic<Stmt> Parser::declaration() {
    //try {
    if (match<TokenType::CLASS>()) return xyz::make_polymorphic<Stmt>(classDeclaration());
    if (match<TokenType::FUN>()) return xyz::make_polymorphic<Stmt>(function("function"));
    if (match<TokenType::VAR>()) return xyz::make_polymorphic<Stmt>(varDeclaration());
    return statement();
    //}
    //catch (ParseError const& error) {
    //    synchronise();
    //    return nullptr;
    //}
}

ClassStmt Parser::classDeclaration() {
    auto const name = consume<TokenType::IDENTIFIER>("Expect class name.");

    const auto superclass = match<TokenType::LESS>() ? [&] {
        consume<TokenType::IDENTIFIER>("Expect superclass name.");
        return new VariableExpr(previous());
    }() : nullptr;

    consume<TokenType::LEFT_BRACE>("Expect '{' before class body.");

    std::vector<FunctionStmt> methods;
    while (!check<TokenType::RIGHT_BRACE>() && !isAtEnd()) {
        methods.push_back(function("method"));
    }

    consume<TokenType::RIGHT_BRACE>("Expect '}' after class body.");

    return ClassStmt(name, superclass, methods);
}

VarStmt Parser::varDeclaration() {
    auto const& name = consume<TokenType::IDENTIFIER>("Expect variable name");
    auto const initializer = match<TokenType::EQUAL>() ? std::optional(expression()) : std::nullopt;
    consume<TokenType::SEMICOLON>("Expect ';' after variable declaration.");
    return VarStmt(name, initializer);
}

xyz::polymorphic<Stmt> Parser::statement() {
    if (match<TokenType::IF>()) return xyz::make_polymorphic<Stmt>(ifStatement());
    if (match<TokenType::WHILE>()) return xyz::make_polymorphic<Stmt>(whileStatement());
    if (match<TokenType::FOR>()) return forStatement();
    if (match<TokenType::PRINT>()) return xyz::make_polymorphic<Stmt>(printStatement());
    if (match<TokenType::RETURN>()) return xyz::make_polymorphic<Stmt>(returnStatement());
    if (match<TokenType::LEFT_BRACE>()) return xyz::make_polymorphic<Stmt>(blockStatement());
    return xyz::make_polymorphic<Stmt>(expressionStatement());
}

IfStmt Parser::ifStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'if'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after 'if'.");
    auto const thenBranch = statement();
    auto const elseBranch = match<TokenType::ELSE>() ? std::optional(statement()) : std::nullopt;
    return IfStmt(condition, thenBranch, elseBranch);
}

PrintStmt Parser::printStatement() {
    auto const value = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after value.");
    return PrintStmt(value);
}

WhileStmt Parser::whileStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'while'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after condition.");
    auto const body = statement();
    return WhileStmt(condition, body);
}

ReturnStmt Parser::returnStatement() {
    auto const keyword = previous();
    auto value = std::optional<xyz::polymorphic<Expr>>();
    if (!check<TokenType::SEMICOLON>()) {
        value = expression();
    }
    consume<TokenType::SEMICOLON>("Expect ';' after return value.");
    return ReturnStmt(keyword, value);
}

xyz::polymorphic<Stmt> Parser::forStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'for'.");
    
    auto initializer = std::optional<xyz::polymorphic<Stmt>>();
    if (match<TokenType::SEMICOLON>()) {
        initializer = std::nullopt;
    }
    else if (match<TokenType::VAR>()) {
        initializer = xyz::make_polymorphic<Stmt>(varDeclaration());
    }
    else {
        initializer = xyz::make_polymorphic<Stmt>(expressionStatement());
    }

    auto const condition = check<TokenType::SEMICOLON>() ? xyz::make_polymorphic<Expr>(LiteralExpr(true)) : expression();
    consume<TokenType::SEMICOLON>("Expect ';' after condition.");

    auto const increment = check<TokenType::RIGHT_PAREN>() ? std::optional<xyz::polymorphic<Expr>>() : expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after for clause.");

    auto body = statement();

    if (increment) {
        body = xyz::make_polymorphic<Stmt>(BlockStmt({ body, xyz::make_polymorphic<Stmt>(ExpressionStmt(*increment)) }));
    }

    body = xyz::make_polymorphic<Stmt>(WhileStmt(condition, body));
    
    if (initializer) {
        body = xyz::make_polymorphic<Stmt>(BlockStmt({ *initializer, body }));
    }

    return body;
}

BlockStmt Parser::blockStatement() {
    auto statements = std::vector<xyz::polymorphic<Stmt>>();

    while (!check<TokenType::RIGHT_BRACE>() && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume<TokenType::RIGHT_BRACE>("Expect '}' after block.");
    return BlockStmt(statements);
}

ExpressionStmt Parser::expressionStatement() {
    auto const expr = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after expression.");
    return ExpressionStmt(expr);
}

FunctionStmt Parser::function(std::string const& kind) {
    auto const name = consume<TokenType::IDENTIFIER>("Expect " + kind + " name.");
    consume<TokenType::LEFT_PAREN>("Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check<TokenType::RIGHT_PAREN>()) {
        do {
            if (parameters.size() >= 255) {
                Lox::error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume<TokenType::IDENTIFIER>("Expect parameter name."));
        } while (match<TokenType::COMMA>());
    }
    consume<TokenType::RIGHT_PAREN>("Expect ')' after " + kind + " name.");
    consume<TokenType::LEFT_BRACE>("Expect '{' before " + kind + " body.");
    auto const body = blockStatement();
    return FunctionStmt(name, parameters, body);
}

xyz::polymorphic<Expr> Parser::expression() {
    return assignment();
}

xyz::polymorphic<Expr> Parser::assignment() {
    auto const expr = oor();

    if (match<TokenType::EQUAL>()) {
        auto const equals = previous();
        auto const value = assignment();

        if (auto const variableExpr = dynamic_cast<VariableExpr const*>(xyz::get_raw_pointer(expr))) {
            auto const name = variableExpr->name();
            return xyz::make_polymorphic<Expr>(AssignExpr(name, value));
        }
        else if (auto const getExpr = dynamic_cast<GetExpr const*>(xyz::get_raw_pointer(expr))) {
            return xyz::make_polymorphic<Expr>(SetExpr(getExpr->object(), getExpr->name(), value));
        }

        throw ParseError(equals, "Invalid assignment target.");
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::oor() {
    auto expr = aand();

    while (match<TokenType::OR>()) {
        auto const operatr = previous();
        auto const right = aand();
        expr = xyz::make_polymorphic<Expr>(LogicalExpr(expr, operatr, right));
    }

    return expr;
}
xyz::polymorphic<Expr> Parser::aand() {
    auto expr = equality();

    while (match<TokenType::AND>()) {
        auto const operatr = previous();
        auto const right = equality();
        expr = xyz::make_polymorphic<Expr>(LogicalExpr(expr, operatr, right));
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::equality() {
    auto expr = comparison();
    while (match<TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL>()) {
        auto const operatr = previous();
        auto const right = comparison();
        expr = xyz::make_polymorphic<Expr>(BinaryExpr(expr, operatr, right));
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::comparison() {
    auto expr = term();

    while (match<TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL>()) {
        auto const operatr = previous();
        auto const right = term();
        expr = xyz::make_polymorphic<Expr>(BinaryExpr(expr, operatr, right));
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::term() {
    auto expr = factor();

    while (match<TokenType::MINUS, TokenType::PLUS>()) {
        auto const operatr = previous();
        auto const right = factor();
        expr = xyz::make_polymorphic<Expr>(BinaryExpr(expr, operatr, right));
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::factor() {
    auto expr = unary();

    while (match<TokenType::SLASH, TokenType::STAR>()) {
        auto const operatr = previous();
        auto const right = unary();
        expr = xyz::make_polymorphic<Expr>(BinaryExpr(expr, operatr, right));
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::unary() {
    if (match<TokenType::BANG, TokenType::MINUS>()) {
        auto const operatr = previous();
        auto const right = unary();
        return xyz::make_polymorphic<Expr>(UnaryExpr(operatr, right));
    }

    return call();
}

xyz::polymorphic<Expr> Parser::call() {
    auto expr = primary();

    while (true) {
        if (match<TokenType::LEFT_PAREN>()) {
            expr = finishCall(expr);
        }
        else if (match<TokenType::DOT>()) {
            auto const name = consume<TokenType::IDENTIFIER>("Expect property name after '.'.");
            expr = xyz::make_polymorphic<Expr>(GetExpr(expr, name));
        }
        else {
            break;
        }
    }

    return expr;
}

xyz::polymorphic<Expr> Parser::finishCall(xyz::polymorphic<Expr> callee) {
    auto arguments = std::vector<xyz::polymorphic<Expr>>();
    if (!check<TokenType::RIGHT_PAREN>())
    {
        do {
            if (arguments.size() >= 255) {
                Lox::error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match<TokenType::COMMA>());
    }

    auto const paren = consume<TokenType::RIGHT_PAREN>("Expect ')' after arguments.");

    return xyz::make_polymorphic<Expr>(CallExpr(callee, paren, arguments));

}   

xyz::polymorphic<Expr> Parser::primary() {
    if (match<TokenType::FALSE>()) return xyz::make_polymorphic<Expr>(LiteralExpr(false));
    if (match<TokenType::TRUE>()) return xyz::make_polymorphic<Expr>(LiteralExpr(true));
    if (match<TokenType::NIL>()) return xyz::make_polymorphic<Expr>(LiteralExpr({}));
    
    if (match<TokenType::SUPER>()) {
        auto const keyword = previous();
        consume<TokenType::DOT>("Expect '.' after super.");
        auto const method = consume<TokenType::IDENTIFIER>("Expect superclass method name.");
        return xyz::make_polymorphic<Expr>(SuperExpr(keyword, method));
    }

    if (match<TokenType::NUMBER, TokenType::STRING>()) {
        return xyz::make_polymorphic<Expr>(LiteralExpr(previous().literal()));
    }

    if (match<TokenType::THIS>()) {
        return xyz::make_polymorphic<Expr>(ThisExpr(previous()));
    }

    if (match<TokenType::IDENTIFIER>()) {
        return xyz::make_polymorphic<Expr>(VariableExpr(previous()));
    }

    if (match<TokenType::LEFT_PAREN>()) {
        auto const expr = expression();
        consume<TokenType::RIGHT_PAREN>("Expect ')' after expression.");
        return xyz::make_polymorphic<Expr>(GroupingExpr(expr));
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

std::vector<xyz::polymorphic<Stmt>> parse(std::vector<Token> const& tokens) {
    try {
        auto parser = Parser(tokens);
        return parser.parse();
    }
    catch (ParseError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}
