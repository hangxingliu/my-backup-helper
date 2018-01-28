#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../libs/rang.hpp"

#include "./build-options.hpp"
#include "./launch-arguments.hpp"
#include "./config-parser.hpp"
#include "./command-generator.hpp"
#include "./bash-completion.hpp"

using namespace std;
using namespace rang;

bool isForCompletion = false;
bool isForPasswordSHA1 = false;

string configFilePath = "";
string configFileContent = "";

/** return FILE pointer and set `configFilePath` to config file real path  */
FILE* getConfigFile() {
	UtilsError error;
	for(auto path: CONFIG_FILES) {
		auto strings = expandPOSIXShellString(path, error);
		if(strings.size() != 1) {
			fprintf(stderr, "\n  errror: invalid build options `CONFIG_FILES`");
			fprintf(stderr, "\n  the count of results expanded from \"%s\" is %zu but not 1",
				path.c_str(), strings.size());
			exit(1);
		}
		configFilePath = strings[0];
		FILE* file = fopen(configFilePath.c_str(), "r");
		if(file == nullptr)
			continue;

		if(!isForCompletion)
			printf("  info: backup config file: %s\n", configFilePath.c_str());
		return file;
	}

	fprintf(stderr, "\n  error: the any one of following config files cannot be found:\n");
	for(auto p: CONFIG_FILES)
		fprintf(stderr, "    %s\n", p.c_str());
	fprintf(stderr, "\n");
	exit(1);
}
void loadConfigFileContent() {
	FILE *file = getConfigFile();
	configFileContent = readString(file);
	fclose(file);
}

void listConfigurations(
	ostream& stream, vector<ConfigItemInfo> configurations, string indent = "") {
	for(const ConfigItemInfo& conf: configurations) {
		stream << indent << style::bold << conf.name << style::reset << ":\t";

		if(conf.sudo) stream << fg::cyan << "(sudo) " << style::reset;
		if(!conf.passwordSHA1.empty()) stream << fg::cyan << "(encrypt) " << style::reset;
		if(!conf.type.empty()) stream << fg::cyan << "(" << conf.type << ") " << style::reset;

		if(conf.description.empty())
			stream << style::dim << "empty description" << style::reset << endl;
		else
			stream << conf.description << endl;
	}
	stream << endl;
}
vector<ConfigItemInfo> findConfigurationsByNames(
	vector<ConfigItemInfo> configurations, vector<string> names) {

	map<string, ConfigItemInfo> allConf;
	for(const ConfigItemInfo& conf: configurations)
		allConf[conf.name] = conf;

	map<string, ConfigItemInfo> resultMap;
	for(auto name: names) {
		auto matched = allConf.find(name);
		if(matched == allConf.end()) {
			cerr << fg::red << "\n  error: unknown backup config name: `" <<
				name << "`" << fg::reset << endl;
			cerr << "\n  available configurations:\n\n";
			listConfigurations(cerr, configurations, "    ");
			exit(1);
		}
		auto storaged = resultMap.find(name);
		if(storaged != resultMap.end())
			continue; // storaged
		resultMap[name] = matched->second;
	}

	vector<ConfigItemInfo> result;
	for(auto kv: resultMap)
		result.push_back(kv.second);
	return result;
}

// ==============================================
//
//       M  a  i  n    F u n c t i o n
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char* argv[]) {

	LaunchArguments argument(argc, argv);
	isForCompletion = argument.isCompletion;
	isForPasswordSHA1 = argument.isTestPassword();

	if(isForCompletion && argument.configurations.size() == 0) {
		// only one word `completion` for displaying bash completion scripts
		cout << BashCompletion::getCompletionScripts();
		return 0;
	}

	if(isForPasswordSHA1) {
		int count = 1;
		cout << "\nPassword to sha1sum REPL:\n";
		while(1) {
			auto pwd = PasswordInput::input(
				std::string("\n  (") + std::to_string(count++) + ") Input plain password: ");
			cout << "\n  sha1sum: " << pwd.sha1 <<
				(pwd.plain.empty() ? " (empty)" : "") << "\n";
		}
		return 0;
	}

	loadConfigFileContent(); // cout << configFileContent << endl;
	ConfigParser parser(configFileContent, configFilePath);
	auto configs = parser.getAllConfigurations();
	if(!parser.getError().empty()) {
		cerr << endl << fg::red << "  error: " << parser.getError() << style::reset << endl;
		return 1;
	}

	if(argument.isList) {
		cout << endl << style::bold << "  available configurations:" << style::reset << endl << endl;
		listConfigurations(cerr, parser.getAllConfigurations(), "    ");
		return 0;
	}

	if(isForCompletion && argument.configurations.size() >= 1) {
		vector<string> result;
		for(const ConfigItemInfo& config: configs)
			result.push_back(config.name);

		auto opts = argument.configurations;
		if(atoi(opts[0].c_str()) == 1) { // completion location is 1
			vector<string> actions = {"password", "sha1sum", "sha1", "completion", "list"};
			result.insert(result.end(), actions.begin(), actions.end());
		}
		if(opts.size() >= 2 && opts[1][0] == '-') {
			vector<string> options = {"-h", "-v", "-V", "--help", "--version", "--verbose"};
			result.insert(result.end(), options.begin(), options.end());
		}
		for(auto word: result)
			cout << word << " ";
		cout << endl;
		return 0;
	}

	auto targetDir = chooseAvailableDir(parser.getTargetDirectories());
	printf("  info: backup to : %s\n", configFilePath.c_str());

	CommandGenerator command;

	auto matchedConfigs = findConfigurationsByNames(configs, argument.configurations);
	for(ConfigItemInfo& config: matchedConfigs) {
		string fileName = config.prefix + getNowDateTimeString() + "." +
			(config.type.empty() ? string(DEFAULT_TYPE) : config.type);

		if(targetDir.back() != '/')
			targetDir += '/';
		fileName = targetDir + fileName;

		cout << endl << style::bold
			<< "  ======  configuration name: " << config.name << "  ========"
			<< style::reset << endl;
		printf("  target file:       %s\n", fileName.c_str());
		printf("  backup files:      %zu\n", config.files.size());
		printf("  excludes/recusive: %zu/%zu\n\n", config.exclude.size(), config.excludeRecursive.size());

		auto _cmd = command.generate(fileName, config);
		const char* cmd = _cmd.c_str();

		if(argument.isVerbose) {
			cout << fg::blue;
			printf("\nVERBOSE INFO >>>\n");
			puts(config.toString("  ").c_str());
			puts("  command:");
			printf("    %s\n", command.generate(fileName, config, true).c_str());
			puts("VERBOSE INFO <<<\n");
			cout << style::reset;
		}

		printf("  info: executing compress command ...\n");
		int code = system(cmd);
		if(code != 0) {
			fprintf(stderr, "\n  error: compress failed!\n  details: %s\n\n", cmd);
			exit(1);
		}
	}
	cout << endl << fg::green << style::bold
		 << "  success: backed up " << matchedConfigs.size() << " configurations!"
		 << style::reset << endl << endl;
	return 0;
}
