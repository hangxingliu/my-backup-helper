#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "./build-options.hpp"
#include "./launch-arguments.hpp"
#include "./config-parser.hpp"
#include "./command-generator.hpp"
#include "./bash-completion.hpp"

using namespace std;

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
	FILE* stream, vector<ConfigItemInfo> configurations, string indent = "") {
	for(const ConfigItemInfo& conf: configurations) {
		string decoration = " ";
		if(!conf.passwordSHA1.empty()) decoration += "(encrpyted) ";
		decoration += conf.description.empty() ? "(empty description)" : conf.description;
		fprintf(stream, "%s%s:%s\n", indent.c_str(), conf.name.c_str(), decoration.c_str());
	}
	fputs("\n", stream);
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
			fprintf(stderr, "\n  error: unknown backup config name: `%s`\n", name.c_str());
			fprintf(stderr, "\n  available:\n");
			listConfigurations(stderr, configurations, "    ");
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
		fprintf(stderr, "\n  error: %s\n", parser.getError().c_str());
		return 1;
	}

	if(isForCompletion && argument.configurations.size() >= 1) {
		vector<string> result;
		for(const ConfigItemInfo& config: configs)
			result.push_back(config.name);

		auto opts = argument.configurations;
		if(atoi(opts[0].c_str()) == 1) { // completion location is 1
			vector<string> actions = {"password", "sha1sum", "sha1", "completion"};
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

		printf("\n  ======  configuration name: %s  ========\n", config.name.c_str());
		printf("  target file:       %s\n", fileName.c_str());
		printf("  backup files:      %zu\n", config.files.size());
		printf("  excludes/recusive: %zu/%zu\n\n", config.exclude.size(), config.excludeRecursive.size());

		auto _cmd = command.generate(fileName, config);
		const char* cmd = _cmd.c_str();

		if(argument.isVerbose) {
			printf("\nVERBOSE INFO >>>\n");
			puts(config.toString("  ").c_str());
			puts("  command:");
			printf("    %s\n", command.generate(fileName, config, true).c_str());
			puts("VERBOSE INFO <<<\n");
		}

		printf("  info: executing compress command ...\n");
		int code = system(cmd);
		if(code != 0) {
			fprintf(stderr, "\n  error: compress failed!\n  details: %s\n\n", cmd);
			exit(1);
		}
	}

	printf("\n  success: backed up %zu configurations!\n\n", matchedConfigs.size());
	return 0;
}
