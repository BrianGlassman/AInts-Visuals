#include "Colony.hpp"

#include "Chamber.hpp"
#include "Hill.hpp"
#include "Tunnel.hpp"

Colony::Colony()
{
    type = 4;
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
void Colony::AddTunnel(float x, float y, float z)
{
    float coords[] = {x, y, z};
    AddTunnel(coords);
}

void Colony::AddChamber(float center[3])
{
    std::shared_ptr<Model> ptr = std::make_shared<Chamber>();
    AddModel(ptr, center);
}
void Colony::AddChamber(float x, float y, float z)
{
    float coords[] = {x, y, z};
    AddChamber(coords);
}

void Colony::AddHill(float center[3])
{
    std::shared_ptr<Model> ptr = std::make_shared<Hill>();
    AddModel(ptr, center);
}
void Colony::AddHill(float x, float y, float z)
{
    float coords[] = {x, y, z};
    AddHill(coords);
}