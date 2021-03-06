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
	std::string generate(const std::string& targetFile, ConfigItemInfo config, bool hidePassword = false) {
		std::vector<std::string> opts;

		opts.push_back("a");
		opts.push_back(targetFile);
		if(!config.passwordSHA1.empty()) {
			auto stored = passwordMap.find(config.passwordSHA1);
			std::string password = "";
			if(stored == passwordMap.end()) {
				const char MAX_TIMES = 3;
				unsigned char currentTimes = 0;

				while(currentTimes++ < MAX_TIMES) {
					auto pwd = PasswordInput::input();
					password = pwd.plain;
					if(pwd.sha1 != config.passwordSHA1) {
						fprintf(stderr, "\n  error: sha1sum(password): %s", pwd.sha1.c_str());
						fprintf(stderr, "\n         but expect is: %s\n", config.passwordSHA1.c_str());

						if(currentTimes < MAX_TIMES) {
							fprintf(stderr, "\n  Sorry, try again:");
							continue;
						}
						fprintf(stderr, "\n  %u incorrect password attempts\n", MAX_TIMES);
						exit(1);
					}

					passwordMap[pwd.sha1] = password;
					break;
				}
			} else {
				password = stored->second;
			}

			if(hidePassword) {
				std::string hiddenPassword = "";
				for(unsigned int i = 0 , j = password.length(); i < j ; i ++ )
					hiddenPassword += '*';
				opts.push_back(std::string("-p") + hiddenPassword);
			} else {
				opts.push_back(std::string("-p") + password);
			}
		}

		// add compressin level options
		if(config.compressionLevel >= 0)
			opts.push_back(std::string("-mx") + std::to_string(config.compressionLevel));

		// add exclude options
		for(auto exclude: config.exclude)
			opts.push_back(std::string("-x!") + exclude);
		for(auto exclude: config.excludeRecursive)
			opts.push_back(std::string("-xr!") + exclude);

		for(auto file: config.files)
			opts.push_back(file);

		std::string prefix = config.sudo ? (std::string("sudo ") + COMPRESSOR): std::string(COMPRESSOR);
		return std::string(prefix) + " " + shellEscape(opts);
	}
};


#endif // CONFIG_TO_COMMAND_HPP
