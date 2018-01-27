#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "../libs/gason.hpp"

using namespace std;

const char* JSON_STR = "{\"num\": 10, \"str\": \"hello!\", \"nil\": null}";

int main(int argc, char **argv) {
	char* jsonStr = strdup(JSON_STR);
	char* endptr;
	JsonAllocator allocator;
	JsonValue object;

	int error = jsonParse(jsonStr, &endptr, &object, allocator);
	if (error != JSON_OK) {
		fprintf(stderr, "%s at %zd\n", jsonStrError(error), endptr - jsonStr);
		exit(EXIT_FAILURE);
	}

	for(auto kv: object) {
		auto key = kv->key;
		auto value = kv->value;

		cout << key << ":" << endl;
		cout << "  as str => " << value.toString() << endl;
		cout << "  as number => " << value.toNumber() << endl;
	}
	return 0;
}
