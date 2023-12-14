#include "Token.h"
#include "TokenType.h"
#include "Object.h"
#include "Parser.h"
#include "Stmt.h"
#include "Lox.h"
#include <catch2/catch_test_macros.hpp>

namespace lox::parser {

    auto const tSEMICOLON = Token(TokenType::SEMICOLON, ";", Object(), 0);
    auto const tEND_OF_FILE = Token(TokenType::END_OF_FILE, "", Object(), 0);

    auto const tPRINT = Token(TokenType::PRINT, "print", Object(), 0);
    auto const tNUMBER = Token(TokenType::NUMBER, "3", Object(3.0), 0);
    
    auto const tVAR = Token(TokenType::VAR, "print", Object(), 0);
    auto const tIDENTIFIER = Token(TokenType::IDENTIFIER, "test", Object(), 0);
    auto const tEQUAL = Token(TokenType::EQUAL, "=", Object(), 0);
    auto const tSTRING = Token(TokenType::STRING, "Test", Object("Test"), 0);
    
    TEST_CASE("Parser produces print statement", "[lox.parser.print]") {
        auto const printNumber = parse({ tPRINT, tNUMBER, tSEMICOLON, tEND_OF_FILE });
        REQUIRE(!Lox::hadError);
        REQUIRE(printNumber.size() == 1);
        REQUIRE(dynamic_cast<PrintStmt const*>(printNumber.front()));
    }

    TEST_CASE("Parser produces var statement", "[lox.parser.var]") {
        auto const printNumber = parse({ tVAR, tIDENTIFIER, tEQUAL, tSTRING, tSEMICOLON, tEND_OF_FILE });
        REQUIRE(!Lox::hadError);
        REQUIRE(printNumber.size() == 1);
        REQUIRE(dynamic_cast<VarStmt const*>(printNumber.front()));
    }

}
