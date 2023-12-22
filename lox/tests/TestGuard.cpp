#include "TestGuard.h"
#include "Lox.h"
#include "Environment.h"
#include <ranges>
#include <cassert>
#include <catch2/catch_test_macros.hpp>

TestGuard::TestGuard() 
    : mOldCout(std::cout.rdbuf(mNewCout.rdbuf())), mOldCerr(std::cerr.rdbuf(mNewCerr.rdbuf())) {
}

std::vector<std::string> TestGuard::capturedLinesCout() {
    auto lines = std::vector<std::string>();
    while (std::getline(mNewCout, lines.emplace_back(), '\n')) {}
    if (lines.back().empty()) lines.pop_back();
    return lines;
}

std::vector<std::string> TestGuard::capturedLinesCerr() {
    auto lines = std::vector<std::string>();
    while (std::getline(mNewCerr, lines.emplace_back(), '\n')) {}
    if (lines.back().empty()) lines.pop_back();
    return lines;
}

TestGuard::~TestGuard() {
    std::cout.rdbuf(mOldCout);
    std::cerr.rdbuf(mOldCerr);

    if (auto lines = capturedLinesCout(); !lines.empty()) {
        std::cerr << "Unhandled captured cout:" << std::endl;
        for (auto const& line : lines) {
            std::cerr << line << std::endl;
        }
    }

    if (auto lines = capturedLinesCerr(); !lines.empty()) {
        std::cerr << "Warning! Unhandled captured cerr:" << std::endl;
        for (auto const& line : lines) {
            std::cerr << line << std::endl;
        }
    }

    Lox::globals = Environment();
    Lox::locals.clear();
    Lox::hadError = false;
}
