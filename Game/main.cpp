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
    LOG.write(utils::LogTarget::Stdout, "PATH: %s") % config.replacePath(paths::system::config);
    LOG.write(utils::LogTarget::Stdout, "PATH: %s") % config.replacePath(paths::system::cache);
    LOG.write(utils::LogTarget::Stdout, "PATH: %s") % config.replacePath(paths::system::prefix);

    SDL_Delay(5000);
    return 0;
}

