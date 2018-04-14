#!/usr/bin/env bash

BUILD_DIR="bin";
BASE_DIR="$(pwd)";

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
fatal() { fail "$1"; exit 1; }
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

doing "checking build target directory";
if [[ ! -e "$BUILD_DIR" ]]; then
	mkdir -p "$BUILD_DIR" && success "created build target directory \"${BUILD_DIR}\"" ||
		fatal "could not create build target directory \"${BUILD_DIR}\"";
else
	success "build target directory \"${BUILD_DIR}\" is existed";
fi


doing "executing cmake";
pushd "$BUILD_DIR";
if [[ -e "Makefile" ]]; then
	success "skip cmake (because Makefile has been generated!)";
else
	cmake "${BASE_DIR}" && success "cmake success!" || fatal "cmake failed!";
fi

doing "making";
make -j4 && success "make success!" || fatal "make failed!";

exit 0;
