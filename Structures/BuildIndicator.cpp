#include "BuildIndicator.hpp"
#include "Shaders.hpp"

#include "Tunnel.hpp"
#include "Chamber.hpp"
#include "Mine.hpp"
#include "Farm.hpp"

BuildIndicator::BuildIndicator()
{
    model = std::make_unique<Chamber>();
    model->Create();
}

void BuildIndicator::Draw()
{
    PushShader(Shader::fixedPipeline);
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    // glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // First draw - Mostly transparent, filled in
    glColor4f(0, 0.5, 1, 0.3);
    model->Draw();

    // Second draw - Opaque, wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(0.3, 0.7, 1, 1.0);
    model->Draw();

    glColor4f(1, 1, 1, 1);
    glPopAttrib();
    PopShader();
}