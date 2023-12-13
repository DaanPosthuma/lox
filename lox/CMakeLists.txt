add_executable (lox "Expr.cpp" "Environment.cpp" "ExprToString.cpp" "Interpreter.cpp" "Lox.cpp" "Object.cpp" "Parser.cpp" "Scanner.cpp" "Stmt.cpp" "Token.cpp" "TokenType.cpp" "LoxCallable.cpp" "Resolver.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.25)
  set_property(TARGET lox PROPERTY CXX_STANDARD 26)
endif()
