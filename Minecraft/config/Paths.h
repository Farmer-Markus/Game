namespace paths {
    namespace system {
    #ifdef __linux__
        constexpr const char* cache = "<HOME>/.cache/Game";
        constexpr const char* config = "<HOME>/.config/Game";
        constexpr const char* prefix = "<HOME>/.local/share/Game";
    #elif _WIN32
        constexpr const char* cache = "<HOME>/AppData/Local/Game/cache";
        constexpr const char* config = "<HOME>/AppData/Local/Game/config";
        constexpr const char* prefix = "<HOME>/AppData/Local/Game";
    #elif __APPLE__
        constexpr const char* cache = "<HOME>/Library/Caches/Game";
        constexpr const char* config = "<HOME>/Library/Application Support/Game";
        constexpr const char* prefix = "<HOME>/Applications/Game.cpp";
    #elif __ANDROID__
        constexpr const char* cache = "<HOME>";
        constexpr const char* config = "<HOME>";
        constexpr const char* prefix = "<HEOME>";
    #endif

    }

    namespace game {
        constexpr const char* data = "<WORKING>/data";
        constexpr const char* latestLog = "<PREFIX>/latest.log";
    }

}