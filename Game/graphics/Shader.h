#include <filesystem>


namespace fs = std::filesystem;

class Shader {
public:
    Shader() {};

    // Shader code from variable
    Shader(const char* vertSrc, const char* fragSrc);

    // Shader code from file
    Shader(const fs::path vertPath, const fs::path fragPath);

    ~Shader();

    // Make return shader program when using glUseProgram(shaderInstance)
    operator unsigned int() const {
        return shaderProgram;
    }

    // To manually set shaderProgram to another value eg. 0
    void operator=(const unsigned int val) {
        shaderProgram = val;
    }

    // Delete shader program
    void destroy();

    // From variable
    void load(const char* vertSrc, const char* fragSrc);

    // From file
    void load(const fs::path vertPath, const fs::path fragPath);
    

private:

    unsigned int shaderProgram = 0;
};
