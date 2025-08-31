namespace paths {
    namespace system {
    #ifdef __linux__
        constexpr const char* cache = "<HOME>/.cache/Game";
        constexpr const char* config = "<HOME>/.config/Game";
        constexpr const char* prefix = "<HOME>/.local/share/Game";
    #elif __WIN32
        constexpr const char* cache = "<APPDATA>/Game/Cache";
        constexpr const char* config = "<APPDATA>/Game";
        constexpr const char* prefix = "<APPDATA>/Game";
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