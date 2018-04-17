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
	bool isCompletion = false;
	bool isList = false;
	bool isTestPasswordSha1 = false;
	std::vector<std::string> configurations;

	static void printHelp() {
		printf("\n  Version: %s (date: %s)\n", VERSION, VERSION_DATE);
		printf("\n  Usage: %s [options] <configuration>...  # backup", PROGRAM_NAME);
		printf("\n         %s password|sha1|sha1sum         # test password sha1sum", PROGRAM_NAME);
		printf("\n         %s list [-v|--verbose]          # list all available configs\n", PROGRAM_NAME);
		printf("\n  Options:\n\n");
		printf("    -h, --help                 output usage information\n");
		printf("    -V, --version              output the version number\n");
		printf("    -v, --verbose              output verbose information in backup\n");
		printf("\n  Advanced:\n\n");
		printf("    Install bash completion: `%s completion >> ~/.bashrc`\n", PROGRAM_NAME);
		printf("    Bash completion principle: `%s completion ${COMP_CWORD} ${CURRENT_WORD}`\n", PROGRAM_NAME);
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

		bool afterDoubleDash = false;
		for(int i = 1 ; i < argc ; i++) {
			const char* opt = argv[i];
			int len = strlen(opt);

			if(len == 0) continue;
			if(strcmp(opt, "--") == 0) {
				afterDoubleDash = true;
				continue;
			}

			if(i == 1) {
				if(strcmp(opt, "list") == 0) {
					isList = true;
					return;
				}
				if(strcmp(opt, "password") == 0 ||
					strcmp(opt, "sha1") == 0 ||
					strcmp(opt, "sha1sum") == 0) {
					isTestPasswordSha1 = true;
					return;
				}
				if(strcmp(opt, "completion") == 0) {
					isCompletion = true;
					for(int j = i + 1; j < argc ; j++)
						configurations.push_back(argv[j]);
					return;
				}
			}

			if(opt[0] == '-' && !afterDoubleDash) {
				if(strcmp(opt, "-h") == 0 || strcmp(opt, "--help") == 0)
					printHelp(); // exit(0);
				if (strcmp(opt, "-V") == 0 || strcmp(opt, "--version") == 0)
					printVersion(); // exit(0);

				if(strcmp(opt, "-v") == 0 || strcmp(opt, "--verbose") == 0) {
					isVerbose = true;
					continue;
				}

				fprintf(stderr, "\n  error: unknown option `%s`\n\n", opt);
				exit(1);
			} else {
				configurations.push_back(std::string(opt));
			}
		}
	}
};

#endif // LAUNCHARGUMENTS_HPP
