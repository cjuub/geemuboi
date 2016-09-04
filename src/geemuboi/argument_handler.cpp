#include "argument_handler.h"

#include <iostream>
#include <cstring>

using namespace std;

ArgumentHandler::ArgumentHandler(int argc, char* argv[]) {
    for (int i = 0; i != argc; ++i) {
        parse_command(i, argv);
    }
}

void ArgumentHandler::parse_command(int& pos, char* argv[]) {
    unique_ptr<Command> cmd = create_command(pos, argv);
    cmd->execute();
}

const string& ArgumentHandler::get_bios() const {
    return bios;
}

const string& ArgumentHandler::get_rom() const {
    return rom;
}

unordered_set<uint16_t>& ArgumentHandler::get_breakpoints() {
    return breakpoints;
}

unique_ptr<ArgumentHandler::Command> ArgumentHandler::create_command(int& pos, char* argv[]) {
    if (strcmp(argv[pos], CMD_TYPE_BIOS) == 0) {
        return unique_ptr<BiosCommand>(new BiosCommand(*this, argv[++pos]));
    } else if (strcmp(argv[pos], CMD_TYPE_ROM) == 0) {
        return unique_ptr<RomCommand>(new RomCommand(*this, argv[++pos]));
    } else if (strcmp(argv[pos], CMD_TYPE_BREAKPOINTS) == 0) {
        return unique_ptr<BreakpointCommand>(new BreakpointCommand(*this, argv[++pos]));
    } else {
        cout << "Illegal argument: " << argv[pos] << endl;
        exit(0);
    }
}

