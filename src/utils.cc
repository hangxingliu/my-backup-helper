#include <stdio.h>
#include <wordexp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <string.h>

#include<string>
#include<vector>

#include "./utils.hpp"

#include "../libs/gason.hpp"

std::vector<std::string> expandPOSIXShellString(const std::string& string, UtilsError& error) {
	std::vector<std::string> result;

	wordexp_t exp;

	error.reset();
	int errorCode = wordexp(string.c_str(), &exp, 0);
	if(errorCode == 0) {
		char** strings = exp.we_wordv;
		for (unsigned int i = 0; i < exp.we_wordc; i++)
			result.push_back(std::string(strings[i]));
		wordfree(&exp);
		return result;
	}

	switch(errorCode) {
		case WRDE_BADCHAR:
			error.setError(errorCode, "Illegal character: |&;<>(){}\\n\\r"); break;
		case WRDE_BADVAL:
			error.setError(errorCode, "An undefined shell variable was referenced"); break;
		case WRDE_CMDSUB:
			error.setError(errorCode, "Command substitution requested"); break;
		case WRDE_NOSPACE:
			error.setError(errorCode, "Out of memory"); break;
		case WRDE_SYNTAX:
			error.setError(errorCode, "Shell syntax error, \
	such as unbalanced parentheses or unmatched quotes."); break;
		default:
			error.setError(errorCode, "Unknown error code");
	}

	return result;
}
std::vector<std::string> expandPOSIXShellStrings(std::vector<std::string> strings, UtilsError& error) {
	std::vector<std::string> results;
	for(auto string: strings) {
		std::vector<std::string> result = expandPOSIXShellString(string, error);
		if(error.hasError) {
			error.setError(error.errorCode, std::string("\"") + string + "\" is invalid: "
				+ error.description);
			break;
		}
		results.insert(results.end(), result.begin(), result.end());
	}
	return results;
}

bool isStringArrayValue(JsonValue& value, const std::string& valueName, UtilsError& error) {
	std::string errPrefix = std::string("`") + valueName;
	error.reset();

	if(value.getTag() != JSON_ARRAY)
		return error.setError(1, errPrefix + "` is not an array");

	for(auto kv: value) {
		const char* key = kv->key;
		JsonValue value = kv->value;
		if(value.getTag() != JSON_STRING)
			return error.setError(1, errPrefix + "[" + key + "]` is a non-string value");
	}

	return true;
}

std::vector<std::string> getStringArrayValues(JsonValue& value) {
	std::vector<std::string>  result;
	for(auto kv: value) {
		JsonValue item = kv->value;
		result.push_back(std::string(item.toString()));
	}
	return result;
}

std::string readString(FILE* file) {
	std::string result = "";
	char buffer[4097];
	int readLen = 0;
	while((readLen = fread(buffer, 1, 4096, file)) > 0 ) {
		buffer[readLen] = 0;
		result += std::string(buffer);
	}
	return result;
}

std::string getJsonType(JsonValue& value) {
	switch (value.getTag()) {
		case JSON_NUMBER: return "number";
		case JSON_STRING: return "string";
		case JSON_ARRAY: return "array";
		case JSON_OBJECT: return "object";
		case JSON_TRUE: return "true";
		case JSON_FALSE: return "false";
		case JSON_NULL: return "null";
	}
	return "unknown";
}

std::string chooseAvailableDir(std::vector<std::string> dirs) {
	struct stat st;
	for(auto dir: dirs)
		if(stat(dir.c_str(), &st) == 0)
			if(S_ISDIR(st.st_mode))
				return dir;
	fprintf(stderr, "\n  error: all target directories are unavailable:\n\n");
	for(auto dir: dirs)
		fprintf(stderr, "    %s\n", dir.c_str());
	fprintf(stderr, "\n");
	exit(1);
}

std::string getNowDateTimeString() {
	time_t t;
	struct tm* timeInfo;
	char buffer[128];

	if(time(&t) < 0) {
		fprintf(stderr, "\n  error: could not get now time!\n\n");
		exit(1);
	}
	timeInfo = localtime(&t);
	strftime(buffer, sizeof(buffer), "%Y%m%d-%H%M%S", timeInfo);
	return std::string(buffer);
}


const char singleQuote = '\'';
std::string shellEscape(std::vector<std::string> args) {
	// inspired from:
	// https://github.com/xxorax/node-shell-escape/blob/master/shell-escape.js
	std::vector<std::string> escaped;
	for(auto arg: args) {
		const char* reader = arg.c_str();

		char* str = (char*) malloc(arg.length() * 4);
		str[0] = singleQuote;
		char* writer = str + 1;
		while(*reader) {
			if(*reader == singleQuote) {
				// ab'c => 'ab'\''c'
				*writer++ = singleQuote;
				*writer++ = '\\';
				*writer++ = singleQuote;
				*writer++ = singleQuote;
			} else {
				*writer++ = *reader;
			}
			reader++;
		}
		*writer++ = singleQuote;
		*writer = 0;
		escaped.push_back(std::string(str));
		free(str);
	}

	bool isFirst = true;
	std::string result = "";
	for(auto str: escaped) {
		if(!isFirst) result+= " ";
		else isFirst = false;
		result += str;
	}
	return result;
}

