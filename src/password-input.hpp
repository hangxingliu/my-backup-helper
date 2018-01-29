#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "../libs/sha1.hpp"
#include "../libs/rang.hpp"

#ifndef PASSWORD_INPUT_HPP
#define PASSWORD_INPUT_HPP

class PasswordInput {
public:
	std::string plain;
	std::string sha1;
private:
	PasswordInput(const std::string& plain, const std::string& sha1):
		plain(plain), sha1(sha1) {}

public:
	static PasswordInput input(const std::string& prompt = "") {
		if(prompt.empty())
			std::cout << "\n  input: password for encrypting backup file: ";
		else
			std::cout << prompt;
		termios oldt;
		tcgetattr(STDIN_FILENO, &oldt);
		termios newt = oldt;
		newt.c_lflag &= ~ECHO;

		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		std::string pwd;
		std::getline(std::cin, pwd);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

		int len = pwd.length();
		char* hide = (char*) malloc(len + 1);
		for(int i = 0 ; i < len ; i ++ ) hide[i] = '*';
		hide[len] = 0;

		std::cout << std::string(hide) << std::endl;

		SHA1 hash;
		hash.update(pwd);
		auto sha1 = hash.final();

		return PasswordInput(pwd, sha1);
	}


	static int startPwd2Sha1REPL() {
		int count = 1;
		std::cout << rang::style::bold << "\nPassword to sha1sum REPL:" << rang::style::reset << "\n";
		while(1) {
			auto pwd = input( std::string("\n  (") +
				std::to_string(count++) + ") Input plain password: " );

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
