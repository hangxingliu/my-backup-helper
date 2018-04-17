#include <string.h>

#include <string>
#include <utility>
#include <vector>

#include "../libs/gason.hpp"

#include "./config-parser-validator.hpp"
#include "./config-parser-comments.hpp"
#include "./config-item.hpp"
#include "./utils.hpp"


#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

class ConfigParser {

	std::vector<std::string> targetDirectories;
	std::vector<ConfigItemInfo> configurations;

	std::string fileContent;
	std::string filePath;

	CommentsInJSON commentJSON;
	char* jsonStr = nullptr;
	JsonValue json;

	std::string error = "";
	bool setError(const std::string& e) {error = e; return false; }
	bool setInvalidConfig(const std::string& details) {
		return setError(std::string("Invalid config: ") + details + "\n"); }
	bool setInvalidConfig(const std::string& name, const std::string& expect, const std::string& actual) {
		return setError(std::string("Invalid config: `") + name +
			"` is not " + expect + ", but " + actual + ".\n"); }

	bool parsed = false;
	// ================================
	// Parser   >>>>>>>>>>>>>>>>>>>>>>
	bool parseTargetDirArray(JsonValue& targetDir) {
		UtilsError error;
		if(!isStringArrayValue(targetDir, "target_dir", error))
			return setInvalidConfig(error.description);

		targetDirectories = expandPOSIXShellStrings(
			commentJSON.removeCommentsInStringArray(
				getStringArrayValues(targetDir)), error);
		if(error.hasError)
			return setInvalidConfig(error.description + "(" + std::to_string(error.errorCode)+ ")");
		return true;
	}
	bool parseConfigurationsMap(JsonValue& configs) {
		ConfigValidator validator;
		UtilsError error;
		if(configs.getTag() != JSON_OBJECT)
			return setInvalidConfig("configurations", "object", getJsonType(configs));

		for(auto kv: configs) {
			auto level = std::string("configurations.") + kv->key;
			if(commentJSON.isComment(kv->key, kv->value))
				continue;

			JsonValue cfg = kv->value;
			if(cfg.getTag() != JSON_OBJECT)
				return setInvalidConfig(level, "object", getJsonType(cfg));

			ConfigItemInfo config;
			config.name = std::string(kv->key);

			if(config.name.empty())
				return setInvalidConfig("`configurations` has a property with a empty key");

			if(config.name == "all")
				return setInvalidConfig("`configurations` has a property with key is reserved word: \"all\"");

			std::vector<std::string> files;
			for(auto it: cfg) {
				if(commentJSON.isComment(it->key, it->value)) continue;

				int type = validator.getItemPropertyType(kv->key, it->key, it->value);
				switch (type) {

				case ConfigValidator::TYPE_INVALID:
					return setError(validator.getError() + "\n");

				case ConfigValidator::TYPE_SUDO:
					config.sudo = validator.getBoolean(); break;

				case ConfigValidator::TYPE_COMPRESSION_LEVEL:
					config.compressionLevel = validator.getInt(); break;

				case ConfigValidator::TYPE_TYPE:
					config.type = validator.getString(); break;

				case ConfigValidator::TYPE_DESCRIPTION:
					config.description = validator.getString(); break;

				case ConfigValidator::TYPE_PASSWORD_SHA1:
					config.passwordSHA1 = validator.getString(); break;

				case ConfigValidator::TYPE_PREFIX:
					config.prefix = validator.getString();
					for(char ch: config.prefix)
						if(ch == '/' || ch == ':' || ch == '*' || ch == '?')
							return setInvalidConfig(level + ".prefix contains invalid character: (/:*?)");
					break;

				case ConfigValidator::TYPE_FILES:
				case ConfigValidator::TYPE_EXCLUDE:
					files = expandPOSIXShellStrings(
						commentJSON.removeCommentsInStringArray(validator.getStrings()), error);
					if(error.hasError)
						return setInvalidConfig(error.description +
							"(" + std::to_string(error.errorCode)+ ")");

					if(type == ConfigValidator::TYPE_FILES)
						config.files = files;
					else
						config.exclude = files;
					break;

				case ConfigValidator::TYPE_EXCLUDE_RECURSIVE:
					config.excludeRecursive =
						commentJSON.removeCommentsInStringArray(validator.getStrings());
					break;
				}
			}

			if(!config.isValid())
				return setInvalidConfig(std::string("`") + level +
					"` is invalid: " + config.getInvalidReason());
			configurations.push_back(config);
		}
		return true;
	}
	bool parse() {
		if(parsed) return error.empty();

		char* endPtr;
		jsonStr = strdup(fileContent.c_str());
		JsonAllocator allocator;

		int status = jsonParse(jsonStr, &endPtr, &json, allocator);
		parsed = true;

		if(status != JSON_OK) {
			setError(std::string("JSON parse exception: ") +
				jsonStrError(status) + " at " + std::to_string(endPtr - jsonStr));
			return false;
		}

		if(json.getTag() != JSON_OBJECT)
			return setInvalidConfig("root element is not an object");

		for(auto it: json) {
			const char* key = it->key;
			bool ok = false;

			if(commentJSON.isComment(key, it->value))
				continue;

			if(strcmp(key, "target_dir") == 0)
				ok = parseTargetDirArray(it->value);
			else if(strcmp(key, "configurations") == 0)
				ok = parseConfigurationsMap(it->value);
			else
				setInvalidConfig(std::string("unknown property: `") + key +
					"` as type " + getJsonType(it->value));

			if(!ok) return false;
		}
		return true;
	}
	// ================================

public:
	ConfigParser(std::string fileContent, std::string filePath):
		fileContent(std::move(fileContent)), filePath(std::move(filePath)) {}
	~ConfigParser() { if(jsonStr != nullptr) free(jsonStr); }

	std::string getError() { return error; }
	std::vector<std::string> getTargetDirectories() { parse(); return targetDirectories; }
	std::vector<ConfigItemInfo> getAllConfigurations() { parse(); return configurations; }

};

#endif // CONFIG_PARSER_HPP
