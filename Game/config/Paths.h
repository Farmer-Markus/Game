namespace paths {
    namespace system {
    #ifdef __linux__
        constexpr const char* cache = "<HOME>/.cache/Game";
        constexpr const char* config = "<HOME>/.config/Game";
        constexpr const char* prefix = "<HOME>/.local/share/Game";
    #elif _WIN32
        constexpr const char* cache = "<WORKING>/cache";
        constexpr const char* config = "<WORKING>";
        constexpr const char* prefix = "<WORKING>";
    #elif __APPLE__
        constexpr const char* cache = "<HOME>/Library/Caches/Game";
        constexpr const char* config = "<HOME>/Library/Application Support/Game";
        constexpr const char* prefix = "<HOME>/Applications/Game.cpp";
    #elif __ANDROID__
        constexpr const char* cache = "";
        constexpr const char* config = "";
        constexpr const char* prefix = "";
    #endif

    }

}