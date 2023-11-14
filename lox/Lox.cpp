#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string>

#include "Scanner.h"
#include "Token.h"
#include "Lox.h"

void Lox::error(int line, std::string message) {
	report(line, "", message);
}

void Lox::report(int line, std::string where, std::string message) {
	std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
	hadError = true;
}

bool Lox::hadError = false;

namespace {

	void run(std::string source) {
		Scanner scanner(source);
		auto const& tokens = scanner.scanTokens();

		for (auto const& token : tokens) {
			std::cout << token.toString() << std::endl;
		}
	}

	void runFile(std::string fileName) {
		std::ifstream t(fileName);
		std::stringstream buffer;
		buffer << t.rdbuf();
		run(buffer.str());
	}

	void runPrompt() {
		while (true) {
			std::cout << "> ";
			std::string line;
			getline(std::cin, line);
			if (line == "exit" || line == "q") return;
			run(line);
		}
	}

}

int main(int argc, char* argv[])
{
	try {

		if (argc > 2) {
			std::cout << "Usage: lox [script]" << std::endl;
		}
		else if (argc == 2) {
			runFile(argv[1]);
		}
		else {
			runPrompt();
		}

	}
	catch (std::exception const& ex) {
		std::cout << "Unhandled exception: " << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Caught something that's not an exception..." << std::endl;
	}
	return 0;
}
