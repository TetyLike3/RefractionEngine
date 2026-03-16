#include <string>

#include "BaseTechnique.h"

using std::string;

BaseTechnique::BaseTechnique() {
    m_ShaderProgram = 0;
}


BaseTechnique::~BaseTechnique() {
    // Delete the intermediate shader objects that have been added to the program
    // The list will only contain something if shaders were compiled but the object itself
    // was destroyed prior to linking.
    for (ShaderObjectList::iterator it = m_ShaderObjectList.begin(); it != m_ShaderObjectList.end(); it++) {
        glDeleteShader(*it);
    }

    if (m_ShaderProgram != 0) {
        glDeleteProgram(m_ShaderProgram);
        m_ShaderProgram = 0;
    }
}


bool BaseTechnique::Init() {
    if (m_ShaderProgram) {
        glDeleteProgram(m_ShaderProgram);
    }

    m_ShaderProgram = glCreateProgram();

    if (m_ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool BaseTechnique::AddShader(GLenum ShaderType, const char* pFilename) {
    string s;

    if (!Utilities::ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    // Save the shader object - will be deleted in the destructor
    m_ShaderObjectList.push_back(ShaderObj);

    const GLchar* p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = { (GLint)s.size() };

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }

    glAttachShader(m_ShaderProgram, ShaderObj);

    return true;
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool BaseTechnique::Finalize() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(m_ShaderProgram);

    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0) {
        glGetProgramInfoLog(m_ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_ShaderProgram);

    glGetProgramiv(m_ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (Success == 0) {
        glGetProgramInfoLog(m_ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    // Delete the intermediate shader objects that have been added to the program
    for (ShaderObjectList::iterator it = m_ShaderObjectList.begin(); it != m_ShaderObjectList.end(); it++) {
        glDeleteShader(*it);
    }

    m_ShaderObjectList.clear();

//    PrintUniformList();

    return GLCheckError();
}


void BaseTechnique::PrintUniformList() {
    int Count = 0;
    glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORMS, &Count);
    printf("Active Uniforms: %d\n", Count);

    GLint Size;
    GLenum Type;
    const GLsizei BufSize = 16;
    GLchar Name[BufSize];
    GLsizei Length;

    for (int i = 0; i < Count; i++) {
        glGetActiveUniform(m_ShaderProgram, (GLuint)i, BufSize, &Length, &Size, &Type, Name);

        printf("Uniform #%d Type: %u Name: %s\n", i, Type, Name);
    }
}


void BaseTechnique::Enable() {
    glUseProgram(m_ShaderProgram);
}


GLint BaseTechnique::GetUniformLocation(const char* pUniformName) {
    GLuint Location = glGetUniformLocation(m_ShaderProgram, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}