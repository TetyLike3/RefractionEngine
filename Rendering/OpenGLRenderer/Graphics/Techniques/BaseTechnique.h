#pragma once

#include <list>
#include <GLAD/glad.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../../Utilities.h"
#include "../../Constants.h"


class BaseTechnique {
public:

    BaseTechnique();

    virtual ~BaseTechnique();

    virtual bool Init();

    void Enable();

    GLuint GetProgram() const { return m_ShaderProgram; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);

    GLuint m_ShaderProgram = 0;

private:

    void PrintUniformList();

    typedef std::list<GLuint> ShaderObjectList;
    ShaderObjectList m_ShaderObjectList;
};

#ifdef FAIL_ON_MISSING_LOC                  
#define GET_UNIFORM_AND_CHECK(loc, name)    \
    loc = GetUniformLocation(name);         \
    if (loc == INVALID_UNIFORM_LOCATION)    \
        return false;                       
#else
#define GET_UNIFORM_AND_CHECK(loc, name)    \
    loc = GetUniformLocation(name);         
#endif

#define DEF_LOC(name) GLuint name = INVALID_UNIFORM_LOCATION