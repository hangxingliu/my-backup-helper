#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../libs/rang.hpp"

#include "./config-item.hpp"

#ifndef CONFIG_NAME_MATCHER_HPP
#define CONFIG_NAME_MATCHER_HPP

class MatchConfig {
public:
	static std::vector<ConfigItemInfo> byNames(
		const std::vector<ConfigItemInfo>& configurations,
		const std::vector<std::string>& names) {

		std::map<std::string, ConfigItemInfo> allConf;
		for(auto& conf: configurations)
			allConf[conf.name] = conf;

		std::map<std::string, ConfigItemInfo> resultMap;
		for(auto& name: names) {
			auto matched = allConf.find(name);
			if(matched == allConf.end()) {
				std::cerr << rang::fg::red << "\n  error: unknown backup config name: `" <<
					name << "`" << rang::fg::reset << "\n";
				std::cerr << "\n  available configurations:\n\n";
				ConfigItemInfo::printItemsToStream(std::cerr, configurations, "    ");
				exit(1);
			}
			auto duplicated = resultMap.find(name);
			if(duplicated != resultMap.end())
				continue; // duplicated

			resultMap[name] = matched->second;
		}

		std::vector<ConfigItemInfo> result;
		for(auto kv: resultMap)
			result.push_back(kv.second);
		return result;
	}

};

#endif // CONFIG_NAME_MATCHER_HPP
