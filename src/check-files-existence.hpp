#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../libs/rang.hpp"

#include "./config-item.hpp"

#ifndef CHECK_FILES_EXISTENCE_HPP
#define CHECK_FILES_EXISTENCE_HPP

class FilesExistence {
public:

	static bool check(
		const std::vector<ConfigItemInfo>& allConfigs,
		const std::vector<ConfigItemInfo>& matchedConfigs) {

		bool ok = true;
		struct stat st;
		for(const ConfigItemInfo& config: allConfigs) {

			bool isMatched = false;
			for(const ConfigItemInfo& c: matchedConfigs) {
				if(c.name == config.name) {
					isMatched = true;
					break;
				}
			}

			for(const std::string& file: config.files) {
				if(stat(file.c_str(), &st) == 0)
					continue; // exist

				if(isMatched) {
					ok = false;
					std::cerr << rang::fg::red << "  FATAL: ";
				} else {
					std::cerr << rang::fg::yellow << "  WARNING: ";
				}
				std::cerr << "Bad stat of \"" << file << "\": " << strerror(errno);
				std::cerr << " (in config `" << config.name << "`)" << rang::style::reset << "\n";
			}
		}

		return ok;
	}
};

#endif // CHECK_FILES_EXISTENCE_HPP
