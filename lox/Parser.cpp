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
        std::vector<Stmt const*> parse();

    private:
        Stmt const* declaration();
        ClassStmt const* classDeclaration();
        VarStmt const* varDeclaration();
        Stmt const* statement();
        IfStmt const* ifStatement();
        PrintStmt const* printStatement();
        WhileStmt const* whileStatement();
        ReturnStmt const* returnStatement();
        Stmt const* forStatement();
        BlockStmt const* blockStatement();
        ExpressionStmt const* expressionStatement();
        FunctionStmt const* function(std::string const& kind);
        Expr const* expression();
        Expr const* assignment();
        Expr const* oor ();
        Expr const* aand ();
        Expr const* equality();
        Expr const* comparison();
        Expr const* term();
        Expr const* factor();
        Expr const* unary();
        Expr const* call();
        Expr const* finishCall(Expr const* callee);
        Expr const* primary();

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

std::vector<Stmt const*> Parser::parse() {
    auto statements = std::vector<Stmt const*>();
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

Stmt const* Parser::declaration() {
    //try {
    if (match<TokenType::CLASS>()) return classDeclaration();
    if (match<TokenType::FUN>()) return function("function");
    if (match<TokenType::VAR>()) return varDeclaration();
    return statement();
    //}
    //catch (ParseError const& error) {
    //    synchronise();
    //    return nullptr;
    //}
}

ClassStmt const* Parser::classDeclaration() {
    auto const name = consume<TokenType::IDENTIFIER>("Expect class name.");

    const auto superclass = match<TokenType::LESS>() ? [&] {
        consume<TokenType::IDENTIFIER>("Expect superclass name.");
        return new VariableExpr(previous());
    }() : nullptr;

    consume<TokenType::LEFT_BRACE>("Expect '{' before class body.");

    std::vector<FunctionStmt const*> methods;
    while (!check<TokenType::RIGHT_BRACE>() && !isAtEnd()) {
        methods.push_back(function("method"));
    }

    consume<TokenType::RIGHT_BRACE>("Expect '}' after class body.");

    return new ClassStmt(name, superclass, methods);
}

VarStmt const* Parser::varDeclaration() {
    auto const& name = consume<TokenType::IDENTIFIER>("Expect variable name");
    auto const initializer = match<TokenType::EQUAL>() ? expression() : new LiteralExpr({});
    consume<TokenType::SEMICOLON>("Expect ';' after variable declaration.");
    return new VarStmt(name, initializer);
}

Stmt const* Parser::statement() {
    if (match<TokenType::IF>()) return ifStatement();
    if (match<TokenType::WHILE>()) return whileStatement();
    if (match<TokenType::FOR>()) return forStatement();
    if (match<TokenType::PRINT>()) return printStatement();
    if (match<TokenType::RETURN>()) return returnStatement();
    if (match<TokenType::LEFT_BRACE>()) return blockStatement();
    return expressionStatement();
}

IfStmt const* Parser::ifStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'if'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after 'if'.");
    auto const thenBranch = statement();
    auto const elseBranch = match<TokenType::ELSE>() ? statement() : nullptr;
    return new IfStmt(condition, thenBranch, elseBranch);
}

PrintStmt const* Parser::printStatement() {
    auto const value = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after value.");
    return new PrintStmt(value);
}

WhileStmt const* Parser::whileStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'while'.");
    auto const condition = expression();
    consume<TokenType::RIGHT_PAREN>("Expect ')' after condition.");
    auto const body = statement();
    return new WhileStmt(condition, body);
}

ReturnStmt const* Parser::returnStatement() {
    auto const keyword = previous();
    auto value = static_cast<Expr const*>(nullptr);
    if (!check<TokenType::SEMICOLON>()) {
        value = expression();
    }
    consume<TokenType::SEMICOLON>("Expect ';' after return value.");
    return new ReturnStmt(keyword, value);
}

Stmt const* Parser::forStatement() {
    consume<TokenType::LEFT_PAREN>("Expect '(' after 'for'.");
    
    Stmt const* initializer;
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

BlockStmt const* Parser::blockStatement() {
    auto statements = std::vector<Stmt const*>();

    while (!check<TokenType::RIGHT_BRACE>() && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume<TokenType::RIGHT_BRACE>("Expect '}' after block.");
    return new BlockStmt(statements);
}

ExpressionStmt const* Parser::expressionStatement() {
    auto const expr = expression();
    consume<TokenType::SEMICOLON>("Expect ';' after expression.");
    return new ExpressionStmt(expr);
}

FunctionStmt const* Parser::function(std::string const& kind) {
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
    return new FunctionStmt(name, parameters, *body);
}

Expr const* Parser::expression() {
    return assignment();
}

Expr const* Parser::assignment() {
    auto const expr = oor();

    if (match<TokenType::EQUAL>()) {
        auto const equals = previous();
        auto const value = assignment();

        if (auto const variableExpr = dynamic_cast<VariableExpr const*>(expr)) {
            auto const name = variableExpr->name();
            return new AssignExpr(name, value);
        }
        else if (auto const getExpr = dynamic_cast<GetExpr const*>(expr)) {
            return new SetExpr(&getExpr->object(), getExpr->name(), value);
        }

        throw ParseError(equals, "Invalid assignment target.");
    }

    return expr;
}

Expr const* Parser::oor() {
    auto expr = aand();

    while (match<TokenType::OR>()) {
        auto const operatr = previous();
        auto const right = aand();
        expr = new LogicalExpr(expr, operatr, right);
    }

    return expr;
}
Expr const* Parser::aand() {
    auto expr = equality();

    while (match<TokenType::AND>()) {
        auto const operatr = previous();
        auto const right = equality();
        expr = new LogicalExpr(expr, operatr, right);
    }

    return expr;
}

Expr const* Parser::equality() {
    auto expr = comparison();
    while (match<TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL>()) {
        auto const operatr = previous();
        auto const right = comparison();
        expr = new BinaryExpr(expr, operatr, right);
    }

    return expr;
}

Expr const* Parser::comparison() {
    auto expr = term();

    while (match<TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL>()) {
        auto const operatr = previous();
        auto const right = term();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr const* Parser::term() {
    auto expr = factor();

    while (match<TokenType::MINUS, TokenType::PLUS>()) {
        auto const operatr = previous();
        auto const right = factor();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr const* Parser::factor() {
    auto expr = unary();

    while (match<TokenType::SLASH, TokenType::STAR>()) {
        auto const operatr = previous();
        auto const right = unary();
        expr = new BinaryExpr(expr, operatr, right);;
    }

    return expr;
}

Expr const* Parser::unary() {
    if (match<TokenType::BANG, TokenType::MINUS>()) {
        auto const operatr = previous();
        auto const right = unary();
        return new UnaryExpr(operatr, right);
    }

    return call();
}

Expr const* Parser::call() {
    auto expr = primary();

    while (true) {
        if (match<TokenType::LEFT_PAREN>()) {
            expr = finishCall(expr);
        }
        else if (match<TokenType::DOT>()) {
            auto const name = consume<TokenType::IDENTIFIER>("Expect property name after '.'.");
            expr = new GetExpr(expr, name);
        }
        else {
            break;
        }
    }

    return expr;
}

Expr const* Parser::finishCall(Expr const* callee) {
    auto arguments = std::vector<Expr const*>();
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

    return new CallExpr(callee, paren, arguments);

}   

Expr const* Parser::primary() {
    if (match<TokenType::FALSE>()) return new LiteralExpr(false);
    if (match<TokenType::TRUE>()) return new LiteralExpr(true);
    if (match<TokenType::NIL>()) return new LiteralExpr({});
    
    if (match<TokenType::SUPER>()) {
        auto const keyword = previous();
        consume<TokenType::DOT>("Expect '.' after super.");
        auto const method = consume<TokenType::IDENTIFIER>("Expect superclass method name.");
        return new SuperExpr(keyword, method);
    }

    if (match<TokenType::NUMBER, TokenType::STRING>()) {
        return new LiteralExpr(previous().literal());
    }

    if (match<TokenType::THIS>()) {
        return new ThisExpr(previous());
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

std::vector<Stmt const*> parse(std::vector<Token> const& tokens) {
    try {
        auto parser = Parser(tokens);
        return parser.parse();
    }
    catch (ParseError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}
