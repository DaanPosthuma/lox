add_executable (lox "Lox.cpp" "ExprToString.cpp" "Scanner.cpp" "TokenType.cpp" "Token.cpp" "Token.h" "TokenType.h"  "Scanner.h" "Lox.h" "Object.h" "Expr.h" "ExprToString.h" "Dispatcher.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lox PROPERTY CXX_STANDARD 23)
endif()
