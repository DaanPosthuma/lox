add_executable (lox "Lox.cpp" "ExprToString.cpp" "Expr.cpp" "Scanner.cpp" "TokenType.cpp" "Token.cpp" "Parser.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lox PROPERTY CXX_STANDARD 23)
endif()
