#include <graphics/Video.h>
#include <filesystem/Filesystem.h>
#include <config/Config.h>
#include <utils/Log.h>

#include <config/Paths.h>


#include <SDL3/SDL.h>

int main() {
    Video video;
    video.initialize();
    if(!video.isInitialized())
        return 1;

    video.swapBuffers();

    Config config;
    LOG.write(utils::LogTarget::FileErr, "PATH: %s") % config.replacePath("<CONFIG>");
    LOG.write(utils::LogTarget::FileErr, "PATH: %s") % config.replacePath("<CACHE>");
    LOG.write(utils::LogTarget::FileErr, "PATH: %s") % config.replacePath("<PREFIX>");
    LOG.write(utils::LogTarget::FileErr, "PATH: %s") % config.replacePath("<WORKING>/test/LOG.txt");
    LOG.write(utils::LogTarget::FileErr, "PATH: %s") % config.replacePath("<DATA>/assets");


    SDL_Delay(5000);
    return 0;
}

