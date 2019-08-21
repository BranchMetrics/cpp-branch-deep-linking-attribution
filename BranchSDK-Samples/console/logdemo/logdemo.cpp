#include <BranchIO/Log.h>

using namespace BranchIO;

int
main(int argc, char **argv) {
    // Debug and verbose messages compiled out in Release builds
    Log::setLevel(Log::Verbose);

    // This is the default output anyway
    Log::enableConsoleLogging();

    // Suppress colored output
    // Log::enableConsoleLogging(false);

    // Send to the system log
    // Log::enableSystemLogging();

    // Log to a file
    // Log::enableFileLogging("logdemo.log");

    BRANCH_LOG_E("Error");
    BRANCH_LOG_W("Warning");
    BRANCH_LOG_I("Info");
    BRANCH_LOG_D("Debug");
    BRANCH_LOG_V("Verbose");
}
