#include "GeomPassTech.h"

GeomPassTech::GeomPassTech() {
}

bool GeomPassTech::Init() {
    if (!BaseTechnique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "Rendering/Resources/shaders/geomPassShader/vertGeomPass.vert")) {
        return false;
    }


    if (!AddShader(GL_FRAGMENT_SHADER, "Rendering/Resources/shaders/geomPassShader/fragGeomPass.frag")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_WorldMatrixLocation = GetUniformLocation("gWorld");
    m_ColorTexUnitLocation = GetUniformLocation("gColorMap");

    if (m_WVPLocation == INVALID_UNIFORM_LOCATION ||
        m_WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
        m_ColorTexUnitLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    return true;
}


void GeomPassTech::SetWVP(const glm::mat4& WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, &WVP[0][0]);
}


void GeomPassTech::SetWorldMatrix(const glm::mat4& WorldInverse) {
    glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, &WorldInverse[0][0]);
}


void GeomPassTech::SetColorTexUnit(unsigned int TextureUnit) {
    glUniform1i(m_ColorTexUnitLocation, TextureUnit);
}