#include "LightingTechnique.h"

LightingTechnique::LightingTechnique() {
}

bool LightingTechnique::Init() {
    if (!BaseTechnique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "lighting.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "lighting.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_SamplerLocation = GetUniformLocation("gSampler");
    m_DirLightColorLocation = GetUniformLocation("gDirectionalLight.Color");
    m_DirLightAmbientIntensityLocation = GetUniformLocation("gDirectionalLight.AmbientIntensity");

    if (m_DirLightAmbientIntensityLocation == 0xFFFFFFFF ||
        m_WVPLocation == 0xFFFFFFFF ||
        m_SamplerLocation == 0xFFFFFFFF ||
        m_DirLightColorLocation == 0xFFFFFFFF) {
        return false;
    }

    return true;
}

void LightingTechnique::SetWVP(const glm::mat4& WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, &WVP[0][0]);
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit) {
    glUniform1i(m_SamplerLocation, TextureUnit);
}


void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light) {
    glUniform3f(m_DirLightColorLocation, Light.m_LightColor.x, Light.m_LightColor.y, Light.m_LightColor.z);
    glUniform1f(m_DirLightAmbientIntensityLocation, Light.m_LightIntensity);
}