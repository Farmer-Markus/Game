#include "Log.h"

#include <config/Config.h>
#include <config/Paths.h>

#include <string>
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;

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

/*LogMessage::LogMessage(Log& lg, LogTarget tg, const std::string arg)
                : log(lg), target(tg) {
    

}*/

void LogMessage::formatLog(const std::string message) {
    // %s is the placeholder for a string
    size_t ph = logBuffer.find("%s");
    if(ph != std::string::npos) {
        logBuffer.replace(ph, 2, message);

    } else {
        log.write(LogTarget::FileErr, 
            "Log: Failed to replace '%s' with additional message. Did you forget to insert '%s' ?");
        logBuffer.clear();
    }
}

void LogMessage::flush() {
    if((target & LogTarget::File) == LogTarget::File) {
        if(!log.writerInitialized && !log.openWriter()) {
            LOG.write(LogTarget::Stderr, "Log: Failed to open log file. Logs will only be printed to terminal.");

        } else {
            log.logToFile(logBuffer + "\n");
        }

    }
    if((target & LogTarget::Stdout) == LogTarget::Stdout) {
        std::cout << logBuffer << std::endl;
    }
    if((target & LogTarget::Stderr) == LogTarget::Stderr) {
        std::cerr << logBuffer << std::endl;
    }
}

LogMessage Log::write(LogTarget target, const std::string message) {
    return LogMessage(*this, target, message);
}


Log::Log() { //-------------------------------------------------------------------------------
    // WICHTIG: Noch Settings/Config modul erstellen für pfade für z.B. log datei/config datei

}

bool Log::openWriter() {
    fs::path log = CFG.replacePath(paths::game::latestLog);
    logWriter.open(log);
    writerInitialized = true;
    
    return logWriter.is_open();
}

bool Log::logToFile(const std::string& ms) {
    writerLock.lock();
    if(!logWriter.is_open())
        return false;

    logWriter.write(ms.c_str(), ms.size());
    logWriter.flush();
    writerLock.unlock();
    return true;
}

} // namespace utils