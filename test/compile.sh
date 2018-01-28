#!/usr/bin/env bash

# =========================================================
#    Change path to directory the install script located
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
SOURCE="${BASH_SOURCE[0]}";
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )";
  SOURCE="$(readlink "$SOURCE")";
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE";
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )";
cd "$DIR";
# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

# setup color variables
if [[ -t 1 ]]; then
	COLOR_MODE=`tput colors`;
	if [[ -n "$COLOR_MODE" ]] && [[ "$COLOR_MODE" -ge 8 ]]; then
		ERROR="\x1b[31;1m"; SUCCESS="\x1b[32m"; RESET="\x1b[0m";
	fi
fi


function check_compile_result() {
	if [[ $? != 0 ]]; then echo -e "${ERROR}[-] failed!${RESET}"; exit 1; fi
	echo -e "${SUCCESS}[~] success!${RESET}";
}

[[ -d "out" ]] || mkdir "out";

echo "[.] compiling wordexp.cc ...";
g++ "wordexp.cc" "../src/utils.cc" \
	-o "out/wordexp.out" \
	-O2 -Wall;
check_compile_result;

echo "[.] compiling test-gason.cc ...";
g++ "test-gason.cc" "../libs/gason.cc" \
	-o "out/test-gason.out" \
	-O2 -Wall;
check_compile_result;
