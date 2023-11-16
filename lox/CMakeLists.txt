add_executable (lox "Expr.cpp" "ExprToString.cpp" "Interpreter.cpp" "Lox.cpp" "Parser.cpp" "Scanner.cpp" "Token.cpp" "TokenType.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lox PROPERTY CXX_STANDARD 23)
endif()
