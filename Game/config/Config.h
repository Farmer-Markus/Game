#include <string>
#include <unordered_map>
#include <filesystem>

class Config {
public:
    Config();

    static Config& Instance() {
        static Config instance;
        return instance;
    }

    // Can read path like: <HOME>/.local/share/game
    // and replaces <HOME> with the system home path
    // Same with CONFIG, CACHE and DATA(game data folder)
    // WORKING (working directory)
    [[nodiscard]] std::filesystem::path replacePath(const std::string path) const;

    // Replace system path in pathMap
    void replacePathEntry(const std::string key, const std::string path);

    bool prepareGameDirs() const;

private:
    std::string getWorkingPath();
    std::string getHomePath();

    std::unordered_map<std::string, std::string> pathMap;
};

#define CFG Config::Instance()