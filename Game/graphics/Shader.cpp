#include "Shader.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utils/Log.h>
#include <glad/glad.h>
#include <fstream>


Shader::Shader(const char* vertSrc, const char* fragSrc) {
    load(vertSrc, fragSrc);
}

Shader::Shader(const fs::path vertPath, const fs::path fragPath) {
    load(vertPath, fragPath);
}

Shader::~Shader() {
    destroy();
}

void Shader::load(const char* vertSrc, const char* fragSrc) {
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
        LOG.write(utils::LogTarget::FileErr, "Failed to compile fragment shader: %s") % infoLog;
        throw std::runtime_error("Failed to compile fragment shader");
    }

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        LOG.write(utils::LogTarget::FileErr, "Failed to compile vertex shader: %s") % infoLog;
        throw std::runtime_error("Failed to compile vertex shader");
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, frag);
    glAttachShader(m_shaderProgram, vert);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        LOG.write(utils::LogTarget::FileErr, infoLog);
        throw std::runtime_error("Failed to link shaders to program");
    }

    glDeleteShader(frag);
    glDeleteShader(vert);
}

void Shader::load(const fs::path vertPath, const fs::path fragPath) {
    std::stringstream strm;
    std::ifstream in;

    in.open(vertPath);
    if(!in.is_open()) {
        LOG.write(utils::LogTarget::FileErr, "Shader: Failed to open shader file: %s") % vertPath.string();
        throw std::runtime_error("Failed to open vertex shader file");
    }

    strm << in.rdbuf();
    std::string vertSrc = strm.str();
    in.close();
    strm.str("");
    strm.clear();

    in.open(fragPath);
    if(!in.is_open()) {
        LOG.write(utils::LogTarget::FileErr, "Shader: Failed to open shader file: %s") % vertPath.string();
        throw std::runtime_error("Failed to open fragment shader file");
    }

    strm << in.rdbuf();
    std::string fragtSrc = strm.str();
    in.close();
    strm.str("");
    strm.clear();

    load(vertSrc.c_str(), fragtSrc.c_str());
}

// Will crash if GL context is destroyed before this function
void Shader::destroy() {
    if(m_shaderProgram > 0 && glIsProgram(m_shaderProgram)) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
}