#include "Config.h"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <utils/Log.h>
#include <string>

namespace fs = std::filesystem;

Config::Config() {
    
}

std::string Config::getWorkingPath() {
#ifdef DEBUG
    LOG.write(utils::LogTarget::Stderr, "Config: Need to get working path");
#endif
    std::string path = fs::current_path().string();
    pathMap["WORKING"] = path;
    return path;
}

std::string Config::getHomePath() {
#ifdef DEBUG
    LOG.write(utils::LogTarget::Stderr, "Config: Need to get HOME path");
#endif
    const auto home = getenv("HOME");
    std::string homePath;
    if(home == NULL)
        LOG.write(utils::LogTarget::FileErr, "Config: Failed to get Home path");
    else
        homePath = home;
    
    pathMap["HOME"] = homePath;
    return homePath;
}

std::string Config::getAppDataPath() {
#ifdef DEBUG
    LOG.write(utils::LogTarget::Stderr, "Config: Need to get LOCALAPPDATA path");
#endif
    const auto appData = getenv("LOCALAPPDATA");
    std::string appDataPath;
    if(appData == NULL)
        LOG.write(utils::LogTarget::FileErr, "Config: Failed to get LocalAppData path");
    else
        appDataPath = appData;
    
    pathMap["APPDATA"] = appDataPath;
    return appDataPath;
}


[[nodiscard]] fs::path Config::replacePath(const std::string path) {
    std::string finalPath;
    size_t currOffset = 0;

    while(true) {
        size_t begin = path.find('<', currOffset);

        // If not first run append the rest or next part of the path
        if(currOffset > 0) {
            finalPath.append(path.substr(currOffset, begin - currOffset));
        }
        
        if(begin != std::string::npos) {
            // +1 to skip '<' character
            begin++;
            currOffset = begin;
            size_t end = path.find('>', currOffset);

            //std::string t = path.substr(begin, end - begin);
            // Look t up in a list and find system path for it
            std::string key = path.substr(begin, end - begin); // Append system path
            auto it = pathMap.find(key);

            // Not in the path map
            if(it == pathMap.end()) {
                std::string sysPath;

                if (key == "WORKING") {
                    sysPath = getWorkingPath();
                } else if(key == "HOME") {
                    sysPath = getHomePath();
                } else if(key == "APPDATA") {
                    sysPath = getAppDataPath();
                }

                if(sysPath.empty()) {
                    LOG.write(utils::LogTarget::FileErr, "Config: System path of map is empty: %s") % key;
                    break;
                }
                finalPath.append(sysPath);

            } else {
                if(it->second.empty()) {
                    LOG.write(utils::LogTarget::FileErr, "Config: System path of map is empty: %s") % key;
                    break;

                } else
                    finalPath.append(pathMap.at(key));
            }

            // Skip '>' character 
            currOffset = end + 1;
        } else {
            if(currOffset == 0 ) {
                LOG.write(utils::LogTarget::FileErr, "Config: Cannot replace path, no placeholder found");
                return path;
            }
            
            break;
        }
    }

    return fs::path(finalPath).make_preferred();
}