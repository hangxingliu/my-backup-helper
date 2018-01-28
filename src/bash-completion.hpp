// ========================================================
//    WARNING!!  Don't modify this file manually
//        It was generated from file `bash-completion.sh`
// ========================================================

#include <string>

#ifndef BASH_COMPLETION_HPP
#define BASH_COMPLETION_HPP

class BashCompletion {
public:
	static std::string getCompletionScripts() {
		return std::string("\n") +
			"###-begin-backup-helper-completion-###\n" + 
			"#\n" + 
			"# backup-helper command completion script\n" + 
			"#\n" + 
			"# Installation: backup-helper completion >> ~/.bashrc  (or ~/.zshrc)\n" + 
			"# Or, maybe: backup-helper completion > /usr/local/etc/bash_completion.d/backup-helper\n" + 
			"#\n" + 
			"_backup_helper_completion() {\n" + 
			"    COMPLETION_FOR_CMD=\"$1\"; # backup-helper\n" + 
			"    CURRENT_WORD=\"$2\";\n" + 
			"    LAST_WORD=\"$3\";\n" + 
			"\n" + 
			"    CONFIGURATIONS=`backup-helper completion \"${COMP_CWORD}\" \"${CURRENT_WORD}\"`;\n" + 
			"    COMPREPLY=( $( compgen -W \"$CONFIGURATIONS\" -- $CURRENT_WORD ) );\n" + 
			"}\n" + 
			"\n" + 
			"complete -F _backup_helper_completion backup-helper;\n" + 
			"###-end-backup-helper-completion-###\n" + 
			"\n";
	}
};
#endif // BASH_COMPLETION_HPP
