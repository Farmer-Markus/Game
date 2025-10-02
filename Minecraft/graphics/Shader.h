#include <filesystem>
#include <string>


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
    bool load(const char* vertSrc, const char* fragSrc);

    // From file
    bool load(const fs::path vertPath, const fs::path fragPath);

    // Delete shader program
    void destroy();

    const char* getErrorMessage() const {
        return m_errMessage.c_str();
    }
    

private:

    std::string m_errMessage;
    unsigned int m_shaderProgram = 0;
};
