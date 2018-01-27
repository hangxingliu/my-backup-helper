/**
 * @file Build Options
 */
#include <vector>
#include <string>

#include "./build-options.hpp"

const char* PROGRAM_NAME = "backup-helper";
const char* VERSION = "1.0.0";
const char* VERSION_DATE= "2018-01-27";

const std::vector<std::string> CONFIG_FILES = {
	"$HOME/.backup-config.json",
	"/etc/backup-config.json",
	"backup-config.debug.json"
};
const char* DEFAULT_TYPE = "tar";
const char* LOG_FILE = "$HOME/.backup-history.log";

const char* COMPRESSOR = "7za";

