#ifndef JFLX_RUNTIMEFLAGS_HPP
#define JFLX_RUNTIMEFLAGS_HPP

#include <iostream>
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, bool> flags = {
    {"-d", false},
};

/*
 * Logs messages when the -d flag is enabled!
 */
inline void logDebugInfo(const std::string &message)
{
    if (flags["-d"])
    {
        std::cout << "[DEBUG]\t" << message << "\n";
    }
}

/*
 * Activates all Flags, that are given
 */
inline void toggleFlags(const int argc, char *argv[])
{
    std::cout << "Flags: \n";
    for (int i = 0; i < argc; i++)
    {
        std::string flag = argv[i];
        std::cout << "Flag " << i << ": " << argv[i] << "\n";
        flags[flag] = true;
    }
}

#endif // JFLX_RUNTIMEFLAGS_HPP