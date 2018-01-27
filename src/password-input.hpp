#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "../libs/sha1.hpp"

#ifndef PASSWORDINPUT_HPP
#define PASSWORDINPUT_HPP

class PasswordInput {
public:
	std::string plain;
	std::string sha1;
private:
	PasswordInput(std::string plain, std::string sha1):
		plain(plain), sha1(sha1) {}

public:
	static PasswordInput input(std::string prompt = "") {
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

};

#endif // PASSWORDINPUT_HPP
