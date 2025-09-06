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

    // Replaces path placeholders like: <HOME>, <WORKING>, <CONFIG>, <CACHE>, <PREFIX>, <DATA>
    [[nodiscard]] std::filesystem::path replacePath(const std::string path) const;

    // Replace path in pathMap. Used to overwrite paths
    void replacePathEntry(const std::string key, const std::string path);

private:
    bool prepareGameDirs() const;
    std::string getWorkingPath();
    std::string getHomePath();

    std::unordered_map<std::string, std::string> pathMap;
};

#define CFG Config::Instance()