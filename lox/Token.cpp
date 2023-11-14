#include "Token.h"

#include "TokenType.h"

std::string Token::toString() const {

    return ::toString(mTokenType) + " " + mLexeme;
}
