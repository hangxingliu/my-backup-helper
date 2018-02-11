#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <utility>

#include "../libs/sha1.hpp"
#include "../libs/rang.hpp"

#ifndef PASSWORD_INPUT_HPP
#define PASSWORD_INPUT_HPP

class PasswordInput {
public:
	std::string plain;
	std::string sha1;
private:
	PasswordInput(std::string plain, std::string sha1):
		plain(std::move(plain)), sha1(std::move(sha1)) {}

public:
	static PasswordInput input(const std::string& prompt = "") {
		if(prompt.empty())
			std::cout << "\n  input: password for encrypting backup file: ";
		else
			std::cout << prompt;
		termios oldAttr{};
		tcgetattr(STDIN_FILENO, &oldAttr);
		termios newAttr = oldAttr;
		newAttr.c_lflag &= ~ECHO;

		// hide password
		tcsetattr(STDIN_FILENO, TCSANOW, &newAttr);
		std::string pwd;
		std::getline(std::cin, pwd);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldAttr);

		// display *****
		std::cout << std::string(pwd.length(), '*') << std::endl;

		SHA1 hash;
		hash.update(pwd);
		auto sha1 = hash.final();

		return PasswordInput(pwd, sha1);
	}

	static int startPwd2Sha1REPL() {
		int count = 0;
		std::cout << rang::style::bold << "\nPassword to sha1sum REPL:" << rang::style::reset << "\n";
		while(++count) {
			auto pwd = input( std::string("\n  (") +
				std::to_string(count) + ") Input plain password: " );

			std::cout << "\n" << rang::style::bold << "  sha1sum: " << rang::style::reset;
			std::cout << pwd.sha1;
			if(pwd.plain.empty())
				std::cout << rang::style::dim << " (empty)" << rang::style::reset;
			std::cout << "\n";
		}
		return 0;
	}
};

#endif // PASSWORD_INPUT_HPP
