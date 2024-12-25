////////////////////////////////////////////////////////////////////////////////
//
// Shader Program Class
//
// Copyright (c)2022-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglShader.hpp"

#include <fstream>
#include <iostream>
#include <vector>

namespace mgl {

    ////////////////////////////////////////////////////////////////// ShaderProgram

    const std::string ShaderProgram::read(const std::string& filename) {
        std::string line, shader_string;
        std::ifstream ifile(filename);
        if (!ifile.is_open()) {
            std::cerr << "[ERROR] Failed to open shader file: " << filename;
            exit(EXIT_FAILURE);
        }
        while (std::getline(ifile, line)) {
            shader_string += line + "\n";
        }
        return shader_string;
    }

    void ShaderProgram::checkCompilation(const GLuint shader_id,
        const std::string& filename) {
        GLint compiled;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE) {
            GLint length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length);
            glGetShaderInfoLog(shader_id, length, &length, log.data());
            std::cerr << "[" << filename << "] " << std::endl << log.data();
            exit(EXIT_FAILURE);
        }
    }

    void ShaderProgram::checkLinkage() {
        GLint linked;
        glGetProgramiv(ProgramId, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE) {
            GLint length;
            glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length);
            glGetProgramInfoLog(ProgramId, length, &length, log.data());
            std::cerr << "[LINK] " << std::endl << log.data() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ShaderProgram::ShaderProgram() : ProgramId(glCreateProgram()) {}

    ShaderProgram::~ShaderProgram() {
        glUseProgram(0);
        glDeleteProgram(ProgramId);
    }

    void ShaderProgram::addShader(const GLenum shader_type,
        const std::string& filename) {
        const GLuint shader_id = glCreateShader(shader_type);
        const std::string scode = read(filename);
        const GLchar* code = scode.c_str();
        glShaderSource(shader_id, 1, &code, 0);
        glCompileShader(shader_id);
        checkCompilation(shader_id, filename);
        glAttachShader(ProgramId, shader_id);

        Shaders[shader_type] = { shader_id };
    }

    void ShaderProgram::addAttribute(const std::string& name, const GLuint index) {
        if (isAttribute(name)) {
            std::cerr << "[WARNING] Attribute " << name << " already exists"
                << std::endl;
        }
        glBindAttribLocation(ProgramId, index, name.c_str());
        Attributes[name] = { index };
    }

    bool ShaderProgram::isAttribute(const std::string& name) {
        return Attributes.find(name) != Attributes.end();
    }

    void ShaderProgram::addUniform(const std::string& name) {
        if (isUniform(name)) {
            std::cerr << "[WARNING] Uniform " << name << " already exists" << std::endl;
        }
        Uniforms[name] = { -1 };
    }

    bool ShaderProgram::isUniform(const std::string& name) {
        return Uniforms.find(name) != Uniforms.end();
    }

    void ShaderProgram::addUniformBlock(const std::string& name,
        const GLuint binding_point) {
        if (isUniformBlock(name)) {
            std::cerr << "[WARNING] Uniform block " << name << " already exists"
                << std::endl;
        }
        Ubos[name] = { 0, binding_point };
    }

    bool ShaderProgram::isUniformBlock(const std::string& name) {
        return Ubos.find(name) != Ubos.end();
    }

    void ShaderProgram::create() {
        glLinkProgram(ProgramId);
        checkLinkage();
        for (auto& i : Shaders) {
            glDetachShader(ProgramId, i.second);
            glDeleteShader(i.second);
        }

        for (auto& i : Uniforms) {
            i.second.index = glGetUniformLocation(ProgramId, i.first.c_str());
            if (i.second.index < 0)
                std::cerr << "WARNING: Uniform " << i.first << " not found." << std::endl;
        }
        for (auto& i : Ubos) {
            i.second.index = glGetUniformBlockIndex(ProgramId, i.first.c_str());
            if (i.second.index == GL_INVALID_INDEX)
                std::cerr << "WARNING: UBO " << i.first << " not found." << std::endl;
            glUniformBlockBinding(ProgramId, i.second.index, i.second.binding_point);
        }
    }

    void ShaderProgram::createShaderPrograms(std::string vertexName, std::string fragmentName) {
        std::string vertexShader = "../shaders/" + vertexName + ".vert";
        std::string fragmentShader = "../shaders/" + fragmentName + ".frag";

        addShader(GL_VERTEX_SHADER, vertexShader.c_str());
        addShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());

        //addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
        //addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
        //addUniform(mgl::MODEL_MATRIX);
        //addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);

        create();
    }
    
    //////custom shaders
    //phong
    void PhongShader::bind(GLuint texture) { 
        glUseProgram(ProgramId); 
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void PhongShader::unbind() { glUseProgram(0); }

    //toon
    void ToonShader::bind(GLuint texture) { 
        glUseProgram(ProgramId); 
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void ToonShader::unbind() { glUseProgram(0); }
    
    //glass
    void GlassShader::bind(GLuint texture) { 
        glUseProgram(ProgramId); 
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void GlassShader::unbind() { glUseProgram(0); }

    //skybox
    void SkyboxShader::bind(GLuint texture) {
        glUseProgram(ProgramId);
        glDepthMask(GL_FALSE);
        glCullFace(GL_FRONT);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        printf("ola\n");
    }

    void SkyboxShader::unbind() {
        glDepthMask(GL_TRUE);
        glCullFace(GL_BACK);
        glUseProgram(0);
    }

    //outline
    void OutlineShader::bind(GLuint texture) {
        glUseProgram(ProgramId);
        glDepthMask(GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, texture);
        printf("ola\n");
    }

    void OutlineShader::unbind() {
        glDepthMask(GL_TRUE);
        glUseProgram(0);
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl
