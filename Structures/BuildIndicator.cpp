#include "BuildIndicator.hpp"
#include "Shaders.hpp"
#include "util.hpp"

#include "Tunnel.hpp"
#include "Chamber.hpp"
#include "Mine.hpp"
#include "Farm.hpp"
#include "Hill.hpp"

class DeleteViz : public Structure
{
public:
    DeleteViz()
    {
        cube.radius = 0.4;
    }

    void Draw(bool hasControl = true)
    {
        cube.center = center;
        cube.Draw(hasControl);
    }
private:
    Cube cube;
};
std::shared_ptr<DeleteViz> deleteViz = std::make_shared<DeleteViz>();

BuildIndicator::BuildIndicator()
{
    model = nullptr;
}

void BuildIndicator::SetModel(StructureType type)
{
    Globals::toBuild = type;

    switch(type)
    {
    case StructureType::NONE:
        model = nullptr;
        return; // <-- return, not break
    case StructureType::Tunnel:
        model = std::make_shared<Tunnel>();
        break;
    case StructureType::Chamber:
        model = std::make_shared<Chamber>();
        break;
    case StructureType::Mine:
        model = std::make_shared<Mine>();
        break;
    // case StructureType::Farm:
    //     model = std::make_shared<Farm>();
    //     break;
    case StructureType::Hill:
        model = std::make_shared<Hill>();
        break;
    case StructureType::Delete:
        model = colony->GetChild(center);
        return; // <-- return, not break
    default:
        Fatal(999, "BuildIndicator::SetModel Unrecognized StructureType %d\n", type);
    }

    Create();
    ApplyNoise();
}

void BuildIndicator::HandleKey(unsigned char k)
{
    if (Globals::toBuild == StructureType::NONE) return;

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
    Create();
    ApplyNoise();
}

void BuildIndicator::Create()
{
    if (Globals::toBuild == StructureType::Delete)
    {
        // Get the targeted model
        model = colony->GetChild(center);
        if (model == nullptr)
        {
            // Use a visual stand-in to show the cursor location
            model = deleteViz;
            model->center = center;
        }
    }
    else
    {
        // Move the model center
        model->center = center;
        model->Create();
    }
}

void BuildIndicator::ApplyNoise()
{
    if (Globals::toBuild == StructureType::Delete) return;
    model->ApplyNoise();
}

void BuildIndicator::Draw(bool hasControl)
{
    if (model == nullptr) return;
    if (Globals::viewMode == ViewMode::INTERIOR) return;

    PushShader(Shader::fixedPipeline);
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_TEXTURE_2D);

    // First draw - Mostly transparent, filled in
    glPolygonOffset(0, -1); // Make sure it shows even when overlayed on existing geometry
    if (Globals::toBuild == StructureType::Delete) glColor4f(1, 0, 0, 0.2);
    else glColor4f(0, 0.5, 1, 0.2);
    model->Draw(false);

    // Second draw - Opaque, wireframe
    glPolygonOffset(0, -2); // Show over the first draw (matters in wireframe mode)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (Globals::toBuild == StructureType::Delete) glColor4f(1, 0.3, 0.3, 1.0);
    else glColor4f(0.3, 0.7, 1, 1.0);
    model->Draw(false);

    glPolygonOffset(0, 0);
    glColor4f(1, 1, 1, 1);
    glPopAttrib();
    PopShader();

    ErrCheck("BuildIndicator::Draw");
}