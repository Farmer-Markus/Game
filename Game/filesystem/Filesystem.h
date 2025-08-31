#include <filesystem>

namespace filesystem {

class Path{
public:
    Path() {}

    operator const std::filesystem::path&()const {return path;};
    operator std::filesystem::path&(){return path;};

private:
    std::filesystem::path path;

};

}