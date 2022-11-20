#include "Colony.hpp"

#include "Chamber.hpp"
#include "Hill.hpp"
#include "Tunnel.hpp"

Colony::Colony()
{
    type = 4;
}

void Colony::ApplyNoise(Noise* noise, float offset[])
{
    float newOffset[] = {offset[0] + center[0], offset[1] + center[1], offset[2] + center[2]};
	for (auto&& child : children)
	{
		child->ApplyNoise(noise, newOffset);
	}
}

void Colony::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

        for(auto& child : children)
        {
            child->Draw();
        }
    } glPopMatrix();
}

void Colony::AddModel(std::shared_ptr<Model> modelPtr, float center[3])
{
    memcpy(modelPtr->center, center, 3*sizeof(float));
    children.push_back(modelPtr);
}

void Colony::AddTunnel(float center[3])
{
    std::shared_ptr<Model> ptr = std::make_shared<Tunnel>();
    AddModel(ptr, center);
}
void Colony::AddTunnel(float center[3], unsigned char sides)
{
    std::shared_ptr<Model> ptr = std::make_shared<Tunnel>(sides);
    AddModel(ptr, center);
}
void Colony::AddTunnel(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddTunnel(center);
}
void Colony::AddTunnel(float x, float y, float z, unsigned char sides)
{
    std::shared_ptr<Model> ptr = std::make_shared<Tunnel>(sides);
    float center[] = {x, y, z};
    AddModel(ptr, center);
}

void Colony::AddChamber(float center[3])
{
    std::shared_ptr<Model> ptr = std::make_shared<Chamber>();
    AddModel(ptr, center);
}
void Colony::AddChamber(float center[3], unsigned char sides)
{
    std::shared_ptr<Model> ptr = std::make_shared<Chamber>(sides);
    AddModel(ptr, center);
}
void Colony::AddChamber(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddChamber(center);
}
void Colony::AddChamber(float x, float y, float z, unsigned char sides)
{
    std::shared_ptr<Model> ptr = std::make_shared<Chamber>(sides);
    float center[] = {x, y, z};
    AddModel(ptr, center);
}

void Colony::AddHill(float center[3])
{
    std::shared_ptr<Model> ptr = std::make_shared<Hill>();
    AddModel(ptr, center);
}
void Colony::AddHill(float x, float y, float z)
{
    float center[] = {x, y, z};
    AddHill(center);
}