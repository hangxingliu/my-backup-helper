#include <iostream>
#include <vector>
#include <string>

#include "./config-item.hpp"

#ifndef BASH_COMPLETION_HPP
#define BASH_COMPLETION_HPP

class BashCompletion {
public:

	static int completeKeywords(std::vector<ConfigItemInfo> configs,
			int completeIndex, const std::string& currentWord) {
		std::vector<std::string> result;
		for(const ConfigItemInfo& config: configs)
			result.push_back(config.name);

		if(completeIndex == 1) { // completion location is 1
			std::vector<std::string> actions = { "password", "sha1sum", "sha1", "completion", "list" };
			result.insert(result.end(), actions.begin(), actions.end());
		}
		if(currentWord[0] == '-') {
			std::vector<std::string> options = { "-h", "-v", "-V", "--help", "--version", "--verbose" };
			result.insert(result.end(), options.begin(), options.end());
		}
		for(auto word: result)
			std::cout << word << " ";
		std::cout << std::endl;
		return 0;
	}


};

#endif // BASH_COMPLETION_HPP
