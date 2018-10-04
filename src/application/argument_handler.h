#ifndef ARGUMENT_HANDLER_H
#define ARGUMENT_HANDLER_H

#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>

class ArgumentHandler {
    friend class Command;
public:
    ArgumentHandler(int argc, char* argv[]);

    const std::string& get_bios() const;
    const std::string& get_rom() const;
    std::unordered_set<uint16_t>& get_breakpoints();
private:
    class Command {
    public:
        Command(ArgumentHandler& ah) : argument_handler(ah) {};
        virtual ~Command() {}
        virtual void execute() = 0;
    protected:
        ArgumentHandler& argument_handler;
    };

    class BiosCommand : public Command {
    public:
        BiosCommand(ArgumentHandler& ah, const std::string bios_in) : Command(ah), bios(bios_in) {}
        virtual void execute() { argument_handler.bios = bios; }
    private:
        const std::string bios;
    };

    class RomCommand: public Command {
    public:
        RomCommand(ArgumentHandler& ah, const std::string rom_in) : Command(ah), rom(rom_in) {}
        virtual void execute() { argument_handler.rom = rom; }
    private:
        const std::string rom;
    };

    class BreakpointCommand: public Command {
    public:
        BreakpointCommand(ArgumentHandler& ah, const std::string bps_in) : 
            Command(ah), 
            bps(bps_in) {}
        virtual void execute() { 
            std::istringstream iss(bps);
            std::string breakpoint;
            while (std::getline(iss, breakpoint, ',')) {
                uint16_t bp = static_cast<uint16_t>(std::stoul(breakpoint, nullptr, 16));
                argument_handler.breakpoints.insert(bp);
            }
        }
    private:
        const std::string bps;
    };

    constexpr static const char* CMD_TYPE_BIOS = "-b";
    constexpr static const char* CMD_TYPE_ROM = "-r";
    constexpr static const char* CMD_TYPE_BREAKPOINTS = "-bp";

    std::unique_ptr<Command> create_command(int& pos, char* argv[]);
    void parse_command(int& pos, char* argv[]);

    std::string bios;
    std::string rom;
    std::unordered_set<uint16_t> breakpoints;
};

#endif
