add_library(loxlib 
				Environment.cpp 
				Expr.cpp 
				ExprToString.cpp 
				Interpreter.cpp 
				Lox.cpp 
				LoxCallable.cpp 
				Object.cpp 
				Parser.cpp 
				Resolver.cpp
				Scanner.cpp 
				Stmt.cpp 
				Token.cpp 
				TokenType.cpp 
)

add_executable (lox Main.cpp)

target_link_libraries(lox loxlib)

add_subdirectory ("tests")
