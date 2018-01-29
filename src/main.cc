#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../libs/rang.hpp"

#include "./build-options.hpp"
#include "./launch-arguments.hpp"
#include "./config-file-loader.hpp"
#include "./config-parser.hpp"
#include "./config-name-matcher.hpp"
#include "./command-generator.hpp"
#include "./check-files-existence.hpp"

#include "./bash-completion.hpp"
#include "./bash-completion-scripts.hpp"

using namespace std;
using namespace rang;

int main(int argc, char* argv[]) {
	LaunchArguments argument(argc, argv);

	// Launch for test password sha1, open REPL
	if(argument.isTestPasswordSha1)
		return PasswordInput::startPwd2Sha1REPL();

	// `backup-helper completion` => display bash completion scripts
	if(argument.isCompletion && argument.configurations.size() == 0) {
		cout << BashCompletionScripts::get();
		return 0;
	}

	// load all configurations from config file
	auto configFile = ConfigFileLoader::load(argument.isCompletion);
	ConfigParser parser(configFile.content, configFile.path);
	auto configs = parser.getAllConfigurations();

	// check exception in loading procedure
	if(!parser.getError().empty()) {
		cerr << endl << fg::red << "  error: " << parser.getError() << style::reset << endl;
		return 1;
	}

	// `backup-helper list` => list all available configurations
	if(argument.isList) {
		cout << endl << style::bold << "  available configurations:" << style::reset << endl << endl;
		ConfigItemInfo::printItemsToStream(cerr, parser.getAllConfigurations(), "    ");
		return 0;
	}

	// `backup-helper completion ${index} ${currentWordPrefix}`
	//     => to complete word (options, config names ...)
	if(argument.isCompletion && argument.configurations.size() >= 2) {
		auto opts = argument.configurations;
		return BashCompletion::completeKeywords(configs, atoi(opts[0].c_str()), opts[1]);
	}

	// choose an available backup target directory
	auto targetDir = chooseAvailableDir(parser.getTargetDirectories());
	printf("  info: backup to : %s\n", configFile.path.c_str());

	auto matchedConfigs = MatchConfig::byNames(configs, argument.configurations);

	// check are files need to back up existed
	if(!FilesExistence::check(configs, matchedConfigs))
		return 1;

	CommandGenerator command;
	for(ConfigItemInfo& config: matchedConfigs) {
		string fileName = config.getTargetFileName(targetDir);

		cout << endl << style::bold
			<< "  ======  configuration name: " << config.name << "  ========"
			<< style::reset << endl;
		printf("  target file: %s\n", fileName.c_str());

		auto cmd = command.generate(fileName, config);
		if(argument.isVerbose) {
			cout << fg::blue << "\nVERBOSE INFO >>>\n" << config.toString("  ")
				<< "  command:\n    " << command.generate(fileName, config, true)
				<< "\nVERBOSE INFO <<<\n\n" << style::reset;
		}

		printf("  info: executing compress command ...\n");
		int code = system(cmd.c_str());
		if(code != 0) {
			cout << fg::red << "\n  error: compress failed!\n  details: " << cmd << style::reset << "\n\n";
			exit(1);
		}
	}
	cout << endl << fg::green << style::bold
		 << "  success: backed up " << matchedConfigs.size() << " configurations!"
		 << style::reset << endl << endl;
	return 0;
}
