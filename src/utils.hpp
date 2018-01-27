#include <string>
#include <vector>

#include "../libs/gason.hpp"

#ifndef UTILS_HPP
#define UTILS_HPP

class UtilsError {
public:
	bool hasError = false;
	int errorCode = 0;
	std::string description;
	void reset() { hasError = false; errorCode = 0; }
	bool setError(int code, std::string desc) {
		hasError = true;
		errorCode = code;
		description = desc;
		return false;
	}
};

std::vector<std::string> expandPOSIXShellString(std::string string, UtilsError& error);
std::vector<std::string> expandPOSIXShellStrings(std::vector<std::string> strings, UtilsError& error);
bool isStringArrayValue(JsonValue& value, std::string valueName, UtilsError& error);
std::vector<std::string> getStringArrayValues(JsonValue& value);
std::string readString(FILE* file);
std::string getJsonType(JsonValue& value);
std::string chooseAvailableDir(std::vector<std::string> dirs);
std::string getNowDateTimeString();
std::string shellEscape(std::vector<std::string> args);

#endif // UTILS_HPP
