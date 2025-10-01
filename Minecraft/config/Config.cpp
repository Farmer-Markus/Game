#include "Config.h"
#include "Paths.h"

#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <utils/Log.h>
#include <string>

namespace fs = std::filesystem;

Config::Config() {
    m_pathMap["WORKING"] = getWorkingPath();
    m_pathMap["HOME"] = getHomePath();

    // .string() to make windows compiler happy
    m_pathMap["CONFIG"] = replacePath(paths::system::config).string();
    m_pathMap["CACHE"] = replacePath(paths::system::cache).string();
    m_pathMap["PREFIX"] = replacePath(paths::system::prefix).string();

    m_pathMap["DATA"] = replacePath(paths::game::data).string();

    prepareGameDirs();
}

[[nodiscard]] std::filesystem::path Config::replacePath(const std::string path) const {
    fs::path finalPath;
    size_t currOffset = 0;

    while(true) {
        size_t beg = path.find("<", currOffset);

        // Add next part of path (Not triggered on first run)
        if(currOffset > 0) {
            finalPath += path.substr(currOffset, beg - currOffset);
        }

        // If '<' is found at offset
        if(beg != std::string::npos) {
            // Skip '<' character
            beg++;
            size_t end = path.find(">", currOffset);
            if(end == std::string::npos) {
                LOG.write(utils::LogTarget::FileErr, "Config: Failed to replace path. Placeholder close '>' not found");
                return "";
            }

            currOffset = end + 1; // To make adding the next path(above) not add '>'
            std::string key = path.substr(beg, end - beg);

            auto it = m_pathMap.find(key);
            if(it == m_pathMap.end()) {
                // Path not found in m_pathMap
                LOG.write(utils::LogTarget::FileErr, "Config: Failed to replace path. System path not found in pathMap");
                return "";

            } else
                finalPath += it->second;

        } else { // If '<' not found at offset
            if(currOffset <= 0)
                LOG.write(utils::LogTarget::FileErr, "Config: Failed to replace path. No placeholder found");

            break;
        }

    }

    return finalPath.make_preferred();
}

void Config::replacePathEntry(const std::string key, const std::string path) {
#ifdef DEBUG
    LOG.write(utils::LogTarget::Stdout, "Config: Replacing path entry '%s' with path: %s") % key % path;
#endif
    m_pathMap[key] = path;
}

bool Config::prepareGameDirs() const {
    return fs::create_directories(replacePath(paths::system::config)) &&
        fs::create_directories(replacePath(paths::system::prefix)) &&
        fs::create_directories(replacePath(paths::system::cache));
}

std::string Config::getWorkingPath() {
    std::string path = fs::current_path().string();
    return path;
}

std::string Config::getHomePath() {
#ifdef _WIN32
    const auto home = getenv("USERPROFILE");
#else
    const auto home = getenv("HOME");
#endif
    std::string homePath;
    if(home == NULL || std::string(home).empty()) {
        LOG.write(utils::LogTarget::Stderr, "Config: Failed to get Home path. Using working directory.");
        homePath = getWorkingPath();

    } else
        homePath = home;
    
    return homePath;
}