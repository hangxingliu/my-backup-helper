#include <iostream>
#include <string>
#include <vector>

#include "../libs/rang.hpp"

#include "./build-options.hpp"
#include "./utils.hpp"

#ifndef CONFIGITEM_HPP
#define CONFIGITEM_HPP

class ConfigItemInfo {
	std::string error = "";
	bool setError(const std::string& e) {
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
	std::string toString(const std::string& indent = "") {
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
	void printOneLineToStream(std::ostream& stream, const std::string& indent = "") {
		stream << indent << rang::style::bold << name << rang::style::reset << ":\t";

		if(sudo) stream << rang::fg::cyan << "(sudo) " << rang::style::reset;
		if(!passwordSHA1.empty()) stream << rang::fg::cyan << "(encrypt) " << rang::style::reset;
		if(!type.empty()) stream << rang::fg::cyan << "(" << type << ") " << rang::style::reset;

		if(description.empty())
			stream << rang::style::dim << "empty description" << rang::style::reset << std::endl;
		else
			stream << description << std::endl;
	}
	std::string getTargetFileName(const std::string& targetDir) {
		std::string fileName = prefix + getNowDateTimeString() + "." +
			(type.empty() ? std::string(DEFAULT_TYPE) : type);

		return targetDir + (targetDir.back() != '/' ? "/" : "") + fileName;
	}


	static void printItemsToStream(
			std::ostream& stream,
			std::vector<ConfigItemInfo> confs,
			const std::string& indent = "") {
		for(ConfigItemInfo& conf: confs)
			conf.printOneLineToStream(stream, indent);
		stream << "\n";
	}
};

#endif // CONFIGITEM_HPP
