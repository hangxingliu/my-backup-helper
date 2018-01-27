#include <string>
#include <map>

#include <stdio.h>

#include "./build-options.hpp"
#include "./config-item.hpp"
#include "./utils.hpp"

#include "./password-input.hpp"

#ifndef CONFIG_TO_COMMAND_HPP
#define CONFIG_TO_COMMAND_HPP

class CommandGenerator {
	/// hash to password
	std::map<std::string, std::string> passwordMap;
public:
	std::string generate(std::string targetFile, ConfigItemInfo config) {
		std::vector<std::string> opts;

		opts.push_back("a");
		opts.push_back(targetFile);
		if(!config.passwordSHA1.empty()) {
			auto stored = passwordMap.find(config.passwordSHA1);
			std::string password = "";
			if(stored == passwordMap.end()) {
				auto pwd = PasswordInput::input();
				password = pwd.plain;
				if(pwd.sha1 != config.passwordSHA1) {
					fprintf(stderr, "\n  error: sha1sum(password): %s", pwd.sha1.c_str());
					fprintf(stderr, "\n         but expect is: %s", config.passwordSHA1.c_str());
					exit(1);
				}
				passwordMap[pwd.sha1] = password;
			} else {
				password = stored->second;
			}
			opts.push_back(std::string("-p") + password);
		}

		// add exclude options
		for(auto exclude: config.exclude)
			opts.push_back(std::string("-x!") + exclude);
		for(auto exclude: config.excludeRecursive)
			opts.push_back(std::string("-xr!") + exclude);

		for(auto file: config.files)
			opts.push_back(file);

		return std::string(COMPRESSOR) + " " + shellEscape(opts);
	}
};


#endif // CONFIG_TO_COMMAND_HPP
