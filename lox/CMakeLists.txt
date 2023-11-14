# CMakeList.txt : CMake project for lox, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (lox "Lox.cpp"  "Scanner.cpp" "TokenType.cpp" "Token.cpp" "Token.h" "TokenType.h"  "Scanner.h" "Lox.h" "Object.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lox PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
