/**
 * @file This file is used for skiping the comment fields in config json files
 */

// Because the standard JSON is not support comments.
// Although YAML support comments, but I could not found a lightweight YAML library
//
// So, I decided using special key/value format to define comments in config file:
//
// Rules:
//
//	If parent is an object, comment is a field must conform:
//		1. key string is started with "// "
//		2. value is a string equals "comment"
//	If parent is an array, comment is a string value:
//		started with "// "
//
// For example:
//
//{
//	"// I am a comment": "comment",
//	"target_dir": [
//		"// I am a comment too!"
//		"./demo-env"
//	],
//	"configurations": {
//		"src": {
//			"// I am a comment!"
//			"prefix": "src-",
//			"files": [
//				"// I am a comment too!"
//				"./src",
//				"./libs"
//			]
//		}
//	}
//}

#include "vector"
#include "string"
#include "../libs/gason.hpp"

#ifndef CONFIG_PARSER_COMMENTS_HPP
#define CONFIG_PARSER_COMMENTS_HPP

class CommentsInJSON {
	const std::string prefix = "// ";
	const std::string commentValue = "comment";
	const char* prefixChars = prefix.c_str();
	const unsigned int prefixLen = prefix.length();
public:
	bool isComment(const char* key, JsonValue& value) {
		for(unsigned i = 0 ; i < prefixLen ; i ++ )
			if(!key[i] || key[i] != prefixChars[i])
				return false;
		if(value.getTag() != JSON_STRING)
			return false;
		if(commentValue != value.toString())
			return false;
		return true;
	}
	std::vector<std::string> removeCommentsInStringArray(std::vector<std::string> strings) {
		std::vector<std::string> result;
		for(const std::string& str: strings) {
			if(str.length() >= prefixLen && str.substr(0, prefixLen) == prefix)
				continue; // is comment
			result.push_back(str);
		}
		return result;
	}

};

#endif // CONFIG_PARSER_COMMENTS_HPP
