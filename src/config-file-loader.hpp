#include <iostream>
#include <string>
#include <vector>

#include "../libs/rang.hpp"

#include "./utils.hpp"
#include "./build-options.hpp"

#ifndef CONFIG_FILE_LOADER_HPP
#define CONFIG_FILE_LOADER_HPP

class ConfigFileLoader {

public:
	std::string path;
	std::string content;
	ConfigFileLoader(const std::string& path, const std::string& content):
		path(path), content(content) {}

	static ConfigFileLoader load(bool quiet = false) {
		UtilsError error;
		for(auto path: CONFIG_FILES) {
			auto strings = expandPOSIXShellString(path, error);
			if(strings.size() != 1) {
				std::cerr << "\n" << rang::fg::red
					<< "  error: invalid build options `CONFIG_FILES`\n"
					<< "  the count of results expanded from \"" << path
					<< "\" is " << strings.size() << " but not 1" << rang::style::reset
					<< "\n";
				exit(1);
			}

			auto configPath = strings[0];
			FILE* file = fopen(configPath.c_str(), "r");
			if(file == nullptr)
				continue; // not existed!

			if(!quiet)
				std::cout << "  info: backup config file: " << configPath << "\n";

			auto configContent = readString(file);
			fclose(file);

			return ConfigFileLoader(configPath, configContent);
		}

		std::cerr << "\n" << rang::fg::red
			<< "  error: the any one of following config files cannot be found:"
			<< rang::style::reset
			<< "\n";

		for(auto p: CONFIG_FILES)
			std::cerr << "    " << p << "\n";
		std::cerr << "\n";
		exit(1);
	}

};

#endif // CONFIG_FILE_LOADER_HPP
