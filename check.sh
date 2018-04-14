#!/usr/bin/env bash

DEPS=( "cmake" "make" "g++" "7za" );
DEPS_PACKAGE=( "cmake" "make" "g++" "p7zip-full" );
DEPS_DESCRIPTION=(
	"building source codes (generate Makefile)"
	"building source codes (building/installing follow Makefile)"
	"building source codes (compile c++ codes)"
	"creating backup archive files"
);
DEPS_LEN="${#DEPS[@]}";

# setup color variables
if [[ -t 1 ]]; then
	COLOR_MODE=`tput colors`;
	if [[ -n "$COLOR_MODE" ]] && [[ "$COLOR_MODE" -ge 8 ]]; then
		TITLE="\x1b[36m"; # cyan
		RED="\x1b[31m";
		ERROR="\x1b[31;1m"; # BOLD RED
		SUCCESS="\x1b[32m"; # GREEN
		RESET="\x1b[0m";
	fi
fi
doing() { echo -e "${TITLE}[.] $1 ...${RESET}"; }
fail() { echo -e "${ERROR}[-] fail: $1 ${RESET}"; }
success() { echo -e "${SUCCESS}[~] success: $1 ${RESET}"; }
end() { [[ $1 == 0 ]] &&
	echo -e "${SUCCESS}[+] exit $1${RESET}" ||
	echo -e "${ERROR}[+] exit $1${RESET}"; exit $1; }

# ===================================================================
#        __  __       _             _____       _
#       |  \/  | __ _(_)_ __       | ____|_ __ | |_ _ __ _   _
#       | |\/| |/ _` | | '_ \      |  _| | '_ \| __| '__| | | |
#       | |  | | (_| | | | | |     | |___| | | | |_| |  | |_| |
#       |_|  |_|\__,_|_|_| |_|     |_____|_| |_|\__|_|   \__, |
#                                                        |___/
# ===================================================================

doing "checking dependencies for compiling and runtime";
CODE=0;
for (( i=0 ; i<$DEPS_LEN ; i++ )); do
	DEP_NAME="${DEPS[$i]}";
	if [[ -z `which $DEP_NAME` ]]; then
		fail "\"$DEP_NAME\" is missing! please install package: ${DEPS_PACKAGE[$i]}";
		echo -e "     ${RED}${DEP_NAME} is used for ${DEPS_DESCRIPTION[$i]}${RESET}";
		CODE=1;
	else
		success "\"$DEP_NAME\" is installed!";
	fi
done
end $CODE;

