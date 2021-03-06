/**
 * @file Build Options
 */
#include <vector>
#include <string>

#include "./build-options.hpp"

const char* PROGRAM_NAME = "backup-helper";
const char* VERSION = "1.3.2";
const char* VERSION_DATE= "2018-04-18";

const std::vector<std::string> CONFIG_FILES = { // NOLINT
		"backup-config.debug.json",
	"$HOME/.backup-config.json",
	"/etc/backup-config.json"
};
const char* DEFAULT_TYPE = "tar";
const char* LOG_FILE = "$HOME/.backup-history.log";

const char* COMPRESSOR = "7za";

