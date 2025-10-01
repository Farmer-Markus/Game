#include <filesystem>


namespace fs = std::filesystem;

class Shader {
public:
    ~Shader();

    // Make return shader program when using glUseProgram(shaderInstance)
    operator unsigned int() const {
        return m_shaderProgram;
    }

    // To manually set m_shaderProgram to another value eg. 0
    void operator=(const unsigned int val) {
        m_shaderProgram = val;
    }

    // From variable
    void load(const char* vertSrc, const char* fragSrc);

    // From file
    void load(const fs::path vertPath, const fs::path fragPath);


    // Delete shader program
    void destroy();
    

private:

    unsigned int m_shaderProgram = 0;
};
