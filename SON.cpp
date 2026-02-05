#include "Simulation.h"
#include <iostream>
#include <vector>
#include <string>
#include <atomic>

extern std::atomic<bool> g_slowMode;

void printHelp() {
    std::cout << "Usage: SON.exe [N] [m] [isGeneralLoyal] [--slow]\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h            Show this help message\n";
    std::cout << "  --slow                Enable 1 second delay between messages\n";
    std::cout << "Arguments:\n";
    std::cout << "  N                     Number of nodes (default: 3)\n";
    std::cout << "  m                     Number of faulty nodes (default: 0)\n";
    std::cout << "  isGeneralLoyal        1 for loyal general, 0 for traitor (default: 1)\n";
}

int main(int argc, char* argv[])
{
    int N = 3;
    int m = 0;
    bool isGeneralLoyal = true;
    bool showHelp = false;

    std::vector<std::string> positionalArgs;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        }
        else if (arg == "--slow") {
            g_slowMode = true;
        }
        else if (!arg.empty() && arg[0] == '-') {
            // Unknown option, ignore for now
        }
        else {
            positionalArgs.push_back(arg);
        }
    }

    if (showHelp) {
        printHelp();
        return 0;
    }
    try
    {
        if (positionalArgs.size() > 0) {
            N = std::stoi(positionalArgs[0]);
        }
        if (positionalArgs.size() > 1) {
            m = std::stoi(positionalArgs[1]);
        }
        if (positionalArgs.size() > 2) {
            isGeneralLoyal = (positionalArgs[2] == "1");
        }

        if (!isGeneralLoyal && m == 0) {
            std::cout << "ERROR: m == 0 and isGeneralLoyal == false not compatible" << std::endl;
            return 0;
        }
    }
    catch (const std::exception& e)
    {
        printHelp();
        return 0;
    }

    // Main simulation
    Simulation simulation(N, m, isGeneralLoyal);
    simulation.init();
    simulation.start();
    simulation.end();
}

