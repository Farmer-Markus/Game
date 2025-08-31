#include <string>
#include <unordered_map>
#include <filesystem>

class Config {
public:
    Config();

    // Can read path like: <HOME>/.local/share/game
    // and replaces <HOME> with the system home path
    // Same with CONFIG, CACHE and DATA(game data folder)
    // WORKING (working directory)
    [[nodiscard]] std::filesystem::path replacePath(const std::string path);

private:
    std::string getWorkingPath();
    std::string getHomePath();
    std::string getAppDataPath();

    std::unordered_map<std::string, std::string> pathMap;
};