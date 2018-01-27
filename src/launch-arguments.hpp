#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>

#include "./build-options.hpp"

#ifndef LAUNCHARGUMENTS_HPP
#define LAUNCHARGUMENTS_HPP

class LaunchArguments {
public:
	bool isVerbose = false;
	std::vector<std::string> configurations;

	static void printHelp() {
		printf("\n  Version: %s (date: %s)\n", VERSION, VERSION_DATE);
		printf("\n  Usage: %s [options] <configuration>...  # backup", PROGRAM_NAME);
		printf("\n         %s password|sha1|sha1sum         # test password sha1sum\n", PROGRAM_NAME);
		printf("\n  Options:\n\n");
		printf("    -h, --help                 output usage information\n");
		printf("    -V, --version              output the version number\n");
		printf("    -v, --verbose              output verbose information in backup\n");
		printf("\n");
		exit(0);
	}
	static void printVersion() {
		printf("%s %s (date: %s)\n", PROGRAM_NAME, VERSION, VERSION_DATE);
		exit(0);
	}

	LaunchArguments(int argc, char* argv[]) {
		if(argc <= 1)
			printHelp();

		for(int i = 1 ; i < argc ; i++) {
			const char* opt = argv[i];
			int len = strlen(opt);

			if(len == 0) continue;
			if(strcmp(opt, "-h") == 0 || strcmp(opt, "--help") == 0) {
				printHelp();
			} else if (strcmp(opt, "-V") == 0 || strcmp(opt, "--version") == 0) {
				printVersion();
			} else if(strcmp(opt, "-v") == 0 || strcmp(opt, "--verbose") == 0) {
				isVerbose = true;
				continue;
			} else if(len >= 1 && opt[0] == '-') {
				fprintf(stderr, "\n  error: unknown option `%s`\n\n", opt);
				exit(1);
			} else {
				configurations.push_back(std::string(opt));
			}
		}
	}

	bool isCompletion() {
		return configurations.size() >= 1 && configurations[0] == "completion";
	}
	bool isTestPassword() {
		return configurations.size() >= 1 && (
			configurations[0] == "password" ||
			configurations[0] == "sha1sum" ||
			configurations[0] == "sha1");
	}

};

#endif // LAUNCHARGUMENTS_HPP
