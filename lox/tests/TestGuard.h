#pragma once
#include <sstream>
#include <string>
#include <vector>

class TestGuard {
public:
    TestGuard();
    ~TestGuard();

    std::vector<std::string> capturedLinesCout();
    std::vector<std::string> capturedLinesCerr();

private:
    std::stringstream mNewCout;
    std::stringstream mNewCerr;
    std::streambuf* mOldCout;
    std::streambuf* mOldCerr;

};
