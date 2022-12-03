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

void BuildIndicator::HandleKey(unsigned char k)
{
	switch (k) {
	// Movement
	case 'w':
	case 'W':
		{
            center = center + Vector3Int::Backward;
		}
		break;
	case 'a':
	case 'A':
		{
            center = center + Vector3Int::Left;
		}
		break;
	case 's':
	case 'S':
		{
            center = center + Vector3Int::Forward;
		}
		break;
	case 'd':
	case 'D':
		{
            center = center + Vector3Int::Right;
        }
		break;
    case 'r':
    case 'R':
        {
            center = center + Vector3Int::Up;
        }
        break;
    case 'f':
    case 'F':
        {
            center = center + Vector3Int::Down;
        }
        break;
    default:
        return;
	}

    // If a change was made, have to re-generate
    model->center = center;
    model->Create();
}

void BuildIndicator::Create()
{
    model->center = center;
    model->Create();
}

void BuildIndicator::Draw()
{
    PushShader(Shader::fixedPipeline);
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_TEXTURE_2D);
    glPolygonOffset(0, -1); // Make sure it shows even when overlayed on existing geometry

    // First draw - Mostly transparent, filled in
    glColor4f(0, 0.5, 1, 0.2);
    model->Draw();

    // Second draw - Opaque, wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(0.3, 0.7, 1, 1.0);
    model->Draw();

    glColor4f(1, 1, 1, 1);
    glPopAttrib();
    PopShader();

    ErrCheck("BuildIndicator::Draw\n");
}