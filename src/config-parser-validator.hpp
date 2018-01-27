#include <string.h>

#include <string>

#include "../libs/gason.hpp"
#include "./utils.hpp"

#ifndef CONFIG_PARSER_VALIDATOR_HPP
#define CONFIG_PARSER_VALIDATOR_HPP

class ConfigValidator {
public:
	const static int TYPE_INVALID = -1;
	const static int TYPE_SUDO = 1;
	const static int TYPE_COMPRESSION_LEVEL = 2;
	const static int TYPE_PREFIX = 3;
	const static int TYPE_DESCRIPTION = 4;
	const static int TYPE_TYPE = 5;
	const static int TYPE_FILES = 6;
	const static int TYPE_EXCLUDE = 7;
	const static int TYPE_EXCLUDE_RECURSIVE = 8;
	const static int TYPE_PASSWORD_SHA1 = 9;

private:
	JsonValue lastValue;

	std::string error;
	int setError(std::string e) { error = e; return TYPE_INVALID; }
	int setError(
		std::string prefix, std::string key, std::string expectType, JsonValue& value) {
		return setError(prefix + "." + key + "` is not " + expectType +
			", but actual is " + getJsonType(value));
	}

public:
	std::string getError() { return error; }
	int getItemPropertyType(const char* itemName, const char* key, JsonValue& value) {
		std::string errorPrefix = std::string("Invalid config: configurations.") + itemName + "` ";

		lastValue = value;
		int len = strlen(key);
		if(len == 0)
			return setError(errorPrefix + "` has a empty key property!");

		UtilsError error;
		switch(key[0]) {
		case 's': //sudo
			if(strcmp(key, "sudo") == 0) {
				if(value.getTag() != JSON_TRUE && value.getTag() != JSON_FALSE)
					return setError(errorPrefix, "sudo", "boolean", value);
				return TYPE_SUDO;
			}
			break;
		case 'c': //compress
			if(strcmp(key, "compression_level") == 0) {
				if(value.getTag() != JSON_NUMBER)
					return setError(errorPrefix, "compression_level", "number", value);
				return TYPE_COMPRESSION_LEVEL;
			}
			break;
		case 'd': //description
			if(strcmp(key, "description") == 0) {
				if(value.getTag() != JSON_STRING)
					return setError(errorPrefix, "description", "string", value);
				return TYPE_DESCRIPTION;
			}
			break;
		case 't': //description
			if(strcmp(key, "type") == 0) {
				if(value.getTag() != JSON_STRING)
					return setError(errorPrefix, "type", "string", value);
				return TYPE_TYPE;
			}
			break;
		case 'f': //files
			if(strcmp(key, "files") == 0) {
				if(!isStringArrayValue(value, key, error))
					return setError(errorPrefix, "files", "string[]", value);
				return TYPE_FILES;
			}
			break;
		case 'p': //prefix password_sha1
			if(strcmp(key, "prefix") == 0) {
				if(value.getTag() != JSON_STRING)
					return setError(errorPrefix, "prefix", "string", value);
				return TYPE_PREFIX;
			} else if(strcmp(key, "password_sha1") == 0) {
				if(value.getTag() != JSON_STRING)
					return setError(errorPrefix, "password_sha1", "string", value);
				return TYPE_PASSWORD_SHA1;
			}
			break;
		case 'e': //exclude exclude_recursive
			if(strcmp(key, "exclude") == 0) {
				if(!isStringArrayValue(value, key, error))
					return setError(errorPrefix, "exclude", "string[]", value);
				return TYPE_EXCLUDE;
			} else if(strcmp(key, "exclude_recursive") == 0) {
				if(!isStringArrayValue(value, key, error))
					return setError(errorPrefix, "exclude_recursive", "string[]", value);
				return TYPE_EXCLUDE_RECURSIVE;
			}
			break;
		}

		return setError(errorPrefix + "." + key + "` is invalid property!");
	}

	bool getBoolean() { return lastValue.getTag() == JSON_TRUE; }
	int getInt() { return (int) lastValue.toNumber(); }
	std::string getString() { return lastValue.toString(); }
	std::vector<std::string> getStrings() {
		return getStringArrayValues(lastValue);}
};

#endif // CONFIG_PARSER_VALIDATOR_HPP
