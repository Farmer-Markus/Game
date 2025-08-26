#include "Log.h"

#include <string>
#include <iostream>


namespace utils {

/*
Bitoperator:
    &:
    Compares the bits of two values and returns the bits that are true
    on both values

    010101 & 001111 will return 000101

    |:
    Combines the bits of two values

    01010101 | 10101010 will return 11111111

    <<:
    Shifts given bits to the left
    000001 <<2  returns 000100
*/

void LogMessage::formatLog(const std::string message) {
    size_t t = logBuffer.find("%s");
    if(t != std::string::npos) {
        log.write(LogTarget::Stderr, 
            "Log: Failed to replace '%s' with string. Did you forget to insert '%s' ?");
    } else
        logBuffer.replace(t, 2, message);
}

void LogMessage::flush() {
    if((target & LogTarget::File) == LogTarget::File) {
        log.writeToFile(logBuffer);
    }
    if((target & LogTarget::Stdout) == LogTarget::Stdout) {
        std::cout << logBuffer << "\n";
    }
    if((target & LogTarget::Stderr) == LogTarget::Stderr) {
        std::cerr << logBuffer << "\n";
    }
}

LogMessage Log::write(LogTarget target, const std::string message) {
    return LogMessage(*this, target, message);
}

bool Log::writeToFile(const std::string& ms) {
    if(!logWriter.is_open())
        return false;

    logWriter.write(ms.c_str(), ms.size());
    return true;
}

} // namespace utils