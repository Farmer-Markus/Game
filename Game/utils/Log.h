#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace utils {
    
enum class LogTarget {
    File = 1,   // 001 All can be combined to write to multiple targets
    Stdout = 2, // 010
    Stderr = 4  // 100
};

inline LogTarget operator|(LogTarget first, LogTarget sec){
    return static_cast<LogTarget>(
        static_cast<unsigned>(first) | static_cast<unsigned>(sec));
}

inline LogTarget operator&( LogTarget first, LogTarget sec){
    return static_cast<LogTarget>(
        static_cast<unsigned>(first) & static_cast<unsigned>(sec));
}

class Log;

class LogMessage {
public:
    // std::move to move the string instead of copying
    LogMessage(Log& lg, LogTarget tg, const std::string arg)
                : log(lg), target(tg), logBuffer(std::move(arg)) {};

    ~LogMessage() {
        flush();
    }

    LogMessage& operator%(const std::string message) {
        formatLog(std::move(message));
        return *this;
    }

private:
    void formatLog(const std::string message);
    void flush();


    Log& log;
    LogTarget target;
    std::string logBuffer;
};


class Log {
    friend LogMessage;
public:
    Log() {
        // WICHTIG: Noch Settings/Config modul erstellen für pfade für z.B. log datei/config datei
        fs::path logFile = "GameLog.log";
        if(fs::exists(logFile)) {
            fs::remove(logFile);
        }
        logWriter.open("GameLog.log");
        if(!logWriter.is_open())
            write(LogTarget::Stderr, "Failed to open %s") % logFile.string();
    };

    ~Log() {
        if(logWriter.is_open())
            logWriter.close();
    }

    static Log& Instance() {
        static Log instance;
        return instance;
    }

    LogMessage write(LogTarget target, const std::string message);

private:
    bool writeToFile(const std::string& ms);

    std::ofstream logWriter;

};


} // namespace utils

#define LOG utils::Log::Instance()