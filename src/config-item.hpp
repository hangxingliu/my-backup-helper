#include <string>
#include <vector>

#ifndef CONFIGITEM_HPP
#define CONFIGITEM_HPP

class ConfigItemInfo {
	std::string error = "";
	bool setError(std::string e) {
		e = error;
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
	std::string toString() {
		auto str = std::string("ConfigItemInfo (") + name + ") {\n" +
			"  sudo: " + ( sudo ? "true" : "false" ) + "\"\n" +
			"  prefix: \"" + prefix + "\"\n" +
			"  type: \"" + type + "\"\n" +
			"  description: \"" + description + "\"\n" +
			"  passwordSHA1: \"" + passwordSHA1 + "\"\n" +
			"  compressionLevel: " +
				(compressionLevel < 0 ? "auto" : std::to_string(compressionLevel)) + "\n";

		str += "  files:\n";
		for(auto file: files) str += "    " + file + "\n";

		str += "  excludes:\n";
		for(auto ex: exclude) str += "    " + ex + "\n";
		str += "  excludes(recursive):\n";
		for(auto ex: excludeRecursive) str += "    " + ex + "\n";
		str += "}\n";
		return str;
	}
};

#endif // CONFIGITEM_HPP
