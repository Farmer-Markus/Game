#include "Shader.h"

#include <sstream>
#include <string>
#include <fstream>

#include <utils/Log.h>
#include <glad/glad.h>



Shader::~Shader() {
    destroy();
}

bool Shader::load(const char* vertSrc, const char* fragSrc) {
    unsigned int frag, vert;

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    vert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(frag, 1, &fragSrc, NULL);
    glShaderSource(vert, 1, &vertSrc, NULL);

    glCompileShader(frag);
    glCompileShader(vert);

    int success;
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        m_errMessage = "Fragment shader failed " + std::string(infoLog);
        return false;
    }

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        m_errMessage = "Vertex shader failed " + std::string(infoLog);
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, frag);
    glAttachShader(m_shaderProgram, vert);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        m_errMessage = "Shader program failed " + std::string(infoLog);
        return false;
    }

    glDeleteShader(frag);
    glDeleteShader(vert);
    return true;
}

bool Shader::load(const fs::path vertPath, const fs::path fragPath) {
    std::stringstream strm;
    std::ifstream in;

    in.open(vertPath);
    if(!in.is_open()) {
        m_errMessage = "Could not open '" + fragPath.string() + "'";
        return false;
    }

    strm << in.rdbuf();
    std::string vertSrc = strm.str();
    in.close();
    strm.str("");
    strm.clear();

    in.open(fragPath);
    if(!in.is_open()) {
        m_errMessage = "Could not open '" + vertPath.string() + "'";
        return false;
    }

    strm << in.rdbuf();
    std::string fragtSrc = strm.str();
    in.close();
    strm.str("");
    strm.clear();

    return load(vertSrc.c_str(), fragtSrc.c_str());
}

// Will crash if GL context is destroyed before this function
void Shader::destroy() {
    if(m_shaderProgram > 0 && glIsProgram(m_shaderProgram)) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
}