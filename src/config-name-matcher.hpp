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
		auto isDuplicated = [&](const std::string& name) -> bool {
			return resultMap.find(name) != resultMap.end();
		};

		for(auto& name: names) {
			if(name == "all") {
				for(auto& kv: allConf) {
					if(isDuplicated(kv.first))
						continue;
					resultMap[kv.first] = kv.second;
				}
				continue;
			}

			auto matched = allConf.find(name);
			if(matched == allConf.end()) {
				std::cerr << rang::fg::red << "\n  error: unknown backup config name: `" <<
					name << "`" << rang::fg::reset << "\n";
				std::cerr << "\n  available configurations:\n\n";
				ConfigItemInfo::printItemsToStream(std::cerr, configurations, "    ");
				exit(1);
			}
			if(isDuplicated(name))
				continue;

			resultMap[name] = matched->second;
		}

		std::vector<ConfigItemInfo> result;
		for(auto kv: resultMap)
			result.push_back(kv.second);
		return result;
	}

};

#endif // CONFIG_NAME_MATCHER_HPP
