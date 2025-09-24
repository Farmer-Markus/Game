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
    size_t ph = m_logBuffer.find("%s");
    if(ph != std::string::npos) {
        m_logBuffer.replace(ph, 2, message);

    } else {
        m_log.write(LogTarget::FileErr, 
            "Log: Failed to replace '%s' with additional message. Did you forget to insert '%s' ?");
        m_logBuffer.clear();
    }
}

void LogMessage::flush() {
    if((m_target & LogTarget::File) == LogTarget::File) {
        if(!m_log.m_writerInitialized && !m_log.openWriter()) {
            LOG.write(LogTarget::Stderr, "Log: Failed to open log file. Logs will only be printed to terminal.");

        } else {
            m_log.logToFile(m_logBuffer + "\n");
        }

    }
    if((m_target & LogTarget::Stdout) == LogTarget::Stdout) {
        std::cout << m_logBuffer << std::endl;
    }
    if((m_target & LogTarget::Stderr) == LogTarget::Stderr) {
        std::cerr << m_logBuffer << std::endl;
    }
}

LogMessage Log::write(LogTarget target, const std::string message) {
    return LogMessage(*this, target, message);
}

bool Log::openWriter() {
    fs::path log = CFG.replacePath(paths::game::latestLog);
    m_logWriter.open(log);
    m_writerInitialized = true;
    
    return m_logWriter.is_open();
}

bool Log::logToFile(const std::string& ms) {
    m_writerLock.lock();
    if(!m_logWriter.is_open())
        return false;

    m_logWriter.write(ms.c_str(), ms.size());
    m_logWriter.flush();
    m_writerLock.unlock();
    return true;
}

} // namespace utils