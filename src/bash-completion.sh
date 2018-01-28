#!/usr/bin/env bash

SELF="${BASH_SOURCE[0]}";
TARGET="$(dirname "$SELF")/bash-completion.hpp";
PREFIX='// ========================================================
//    WARNING!!  Don'\''t modify this file manually
//        It was generated from file `bash-completion.sh`
// ========================================================

#include <string>

#ifndef BASH_COMPLETION_HPP
#define BASH_COMPLETION_HPP

class BashCompletion {
public:
	static std::string getCompletionScripts() {
		return std::string("\n") +';

SUFFIX='			"\n";
	}
};
#endif // BASH_COMPLETION_HPP';


echo "$PREFIX" > "$TARGET";
cat "$SELF" |
	awk '/^###-begin-/ { body = 1; } body == 1 { print $0; }' | # slice this file content
	sed 's:\t:    :g' | sed 's:":\\":g' | # escape characters
	awk '{ print "\t\t\t\"" $0 "\\n\" + "}' >> "$TARGET"; # output as c++ codes
echo "$SUFFIX" >> "$TARGET";
echo -e "[+] success: generated to $TARGET";
exit 0;

###-begin-backup-helper-completion-###
#
# backup-helper command completion script
#
# Installation: backup-helper completion >> ~/.bashrc  (or ~/.zshrc)
# Or, maybe: backup-helper completion > /usr/local/etc/bash_completion.d/backup-helper
#
_backup_helper_completion() {
	COMPLETION_FOR_CMD="$1"; # backup-helper
	CURRENT_WORD="$2";
	LAST_WORD="$3";

	CONFIGURATIONS=`backup-helper completion "${COMP_CWORD}" "${CURRENT_WORD}"`;
	COMPREPLY=( $( compgen -W "$CONFIGURATIONS" -- $CURRENT_WORD ) );
}

complete -F _backup_helper_completion backup-helper;
###-end-backup-helper-completion-###
