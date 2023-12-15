include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)
FetchContent_MakeAvailable(Catch2)

include_directories(..)

add_executable(tests TestScanner.cpp TestParser.cpp TestResolver.cpp TestInterpreter.cpp TestFullScript.cpp LogListener.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain loxlib)
