#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"
#include "Lox.h"

std::vector<Token> const& Scanner::scanTokens()
{
    while (!isAtEnd()) {
        mStart = mCurrent;
        scanToken();
    }

    mTokens.push_back(Token(TokenType::END_OF_FILE, {}, {}, mLine));
    return mTokens;
}

void Scanner::scanToken() {
    char c = advance();
    switch (c) {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*': addToken(TokenType::STAR); break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/')) {
            while (peek() != '\n' && !isAtEnd()) advance();
        }
        else {
            addToken(TokenType::SLASH);
        }
    case ' ': [[fallthrough]];
    case '\r': [[fallthrough]];
    case '\t':
        // ignore whitespace
        break;
    case '\n':
        mLine++;
        break;
    case '"': string(); break;
    default:
        if (isdigit(c)) {
            number();
        }
        else if (isalpha(c)) {
            identifier();
        }
        else {
            Lox::error(mLine, std::string("Unexpected character: ") + c);
        }
        break;
    }
}

bool Scanner::isAtEnd() const {
    return mCurrent >= mSource.size();
}

char Scanner::advance() {
    return mSource[mCurrent++];
}

void Scanner::addToken(TokenType type, Object literal) {
    auto const text = mSource.substr(mStart, mCurrent - mStart);
    mTokens.push_back(Token(type, text, literal, mLine));
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (mSource[mCurrent] != expected) return false;

    mCurrent++;
    return true;
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return mSource[mCurrent];
}

char Scanner::peekNext() const {
    if (mCurrent + 1 >= mSource.size()) return '\0';
    return mSource[mCurrent + 1];
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') mLine++;
        advance();
    }

    if (isAtEnd()) {
        Lox::error(mLine, "Unterminated string.");
        return;
    }

    advance(); // the closing "

    auto const value = mSource.substr(mStart + 1, mCurrent - mStart - 2);
    addToken(TokenType::STRING, value);
}

void Scanner::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }

    auto const value = std::stod(mSource.substr(mStart, mCurrent - mStart));

    addToken(TokenType::NUMBER, value);
}

void Scanner::identifier() {
    while (isalnum(peek())) advance();

    auto const text = mSource.substr(mStart, mCurrent - mStart);
    if (auto const it = keywords.find(text); it != keywords.end()) {
        addToken(it->second);
    }
    else {
        addToken(TokenType::IDENTIFIER);
    }
}

std::unordered_map<std::string, TokenType> Scanner::keywords = { 
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE}
};
