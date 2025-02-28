#include <iostream>
#include <string.h>

#include "runtime.hpp"
#include "parser.hpp"

void print_usage() {
    std::cerr << "Usage: steno run <abbreviations file 1> [abbreviations file 2] ... [abbreviations file n]" << std::endl;
    std::cerr << "Usage: steno conflicts <abbreviations file 1> [abbreviations file 2] ... [abbreviations file n]" << std::endl;
}

int main(int arc, char** argv) {
    if (arc < 3) {
        print_usage();
        return 1;
    }

    bool run_command = !strcmp(argv[1], "run");
    bool conflicts_command = !strcmp(argv[1], "conflicts");

    if (!(run_command || conflicts_command)) {
        print_usage();
        return 1;
    }

    Abbreviations abbreviations {};
    for (int i = 2; i < arc; i++) {
        std::string filename {argv[i]};
        parse_abbreviations_file(abbreviations, filename);
    }

    if (run_command) {
        watch_and_expand_abbreviations(abbreviations);
    }
}