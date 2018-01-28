#include <string>
#include <vector>

#ifndef CONFIGITEM_HPP
#define CONFIGITEM_HPP

class ConfigItemInfo {
	std::string error = "";
	bool setError(std::string e) {
		error = e;
		return false;
	}

public:
	bool sudo = false;
	int compressionLevel = -1;
	std::string name = "";
	std::string prefix = "";
	std::string type = "";
	std::string description = "";
	std::string passwordSHA1 = "";
	std::vector<std::string> files;
	std::vector<std::string> exclude;
	std::vector<std::string> excludeRecursive;

	void concatFiles(std::vector<std::string> moreFiles) {
		files.insert(files.end(), moreFiles.begin(), moreFiles.end());
	}
	bool isValid() {
		if(name.empty()) return setError("empty `name`");
		if(prefix.empty()) return setError("empty `prefix`");
		if(files.size() == 0) return setError("empty `files`");
		return true;
	}
	std::string getInvalidReason() {
		return error;
	}
	std::string toString(std::string indent = "") {
		auto str = indent + "ConfigItemInfo (" + name + ") {\n" +
			indent + "  sudo: " + ( sudo ? "true" : "false" ) + "\"\n" +
			indent + "  prefix: \"" + prefix + "\"\n" +
			indent + "  type: \"" + type + "\"\n" +
			indent + "  description: \"" + description + "\"\n" +
			indent + "  passwordSHA1: \"" + passwordSHA1 + "\"\n" +
			indent + "  compressionLevel: " +
				(compressionLevel < 0 ? "auto" : std::to_string(compressionLevel)) + "\n";

		str += indent + "  files:\n";
		for(auto file: files) str += indent + "    \"" + file + "\"\n";

		str += indent + "  excludes:\n";
		for(auto ex: exclude) str += indent + "    \"" + ex + "\"\n";
		str += indent + "  excludes(recursive):\n";
		for(auto ex: excludeRecursive) str += indent + "    \"" + ex + "\"\n";
		str += indent + "}\n";
		return str;
	}
};

#endif // CONFIGITEM_HPP
