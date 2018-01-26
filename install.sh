#!/usr/bin/env bash

BUILD_DIR="bin";
BUILD_TARGET_BIN="backup-helper";

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

doing "checking target binary file";
pushd "$BUILD_DIR";
[[ ! -e "${BUILD_TARGET_BIN}" ]] &&
	fatal "${BUILD_TARGET_BIN} is missing! (have you executed \"./build.sh\")";
success "target binary file \"${BUILD_TARGET_BIN}\" has been generated!";

doing "installing";
make install && success "install success!" || fatal "install failed!";
exit 0;
