#include <fstream>
#include <mutex>
#include <string>


namespace utils {
    
enum class LogTarget {
    File = 1,   // 001 All can be combined to write to multiple targets
    Stdout = 2, // 010
    Stderr = 4,  // 100

    FileErr = File | Stderr
};

// add operator to LogTraget to use with | and &
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
    LogMessage(Log& lg, LogTarget tg, const std::string arg)
                : log(lg), target(tg), logBuffer(arg) {}

    ~LogMessage() {
        flush();
    }

    LogMessage& operator%(const std::string message) {
        formatLog(message);
        return *this;
    }

    LogMessage& operator%(const int message) {
        formatLog(std::to_string(message));
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
    Log();

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
    bool openWriter();
    bool logToFile(const std::string& ms);

    bool writerInitialized = false;
    std::mutex writerLock;
    std::ofstream logWriter;
};


} // namespace utils

#define LOG utils::Log::Instance()