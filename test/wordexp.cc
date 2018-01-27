#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "../src/utils.hpp"

using namespace std;

int main(int argc, char **argv) {

	UtilsError error;
	vector<string> inputs = {
		"$PWD/*",
		"$HOME/*.sh"
	};

	for(auto input : inputs) {
		auto output = expandPOSIXShellString(input, error);

		cout << "input: " << input << endl;
		if(error.hasError) {
			cout << "error: (" << error.errorCode << ") " << error.description << endl;
		} else {
			cout << "output:" << endl;
			for(auto r: output)
				cout << "  \"" << r << "\"" << endl;
		}
		cout << endl;
	}

	return 0;
}
