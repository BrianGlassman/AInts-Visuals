#include "Tunnel.hpp"

#include "globals.hpp"

Tunnel::Tunnel()
{
    type = StructureType::Tunnel;
    sideType = SideType::tunnel;

    Create();
}

void Tunnel::CreateCLHelper(std::vector<Vertex> &CLtoUse, const int axis, const bool flip)
{
    int panels = 4;
    float d = (0.5 - radius) / panels;

    int lastIdx = 0;
    int currentIdx;
    float x = radius;
    for (int i = 0; i <= panels; i++)
    {
        // Create
        currentIdx = CLtoUse.size();
        Vertex vert(currentIdx);
        vert.coords[axis] = (flip ? -1 : 1) * x;

        // Link
        CLtoUse[lastIdx].AddNeighbor(currentIdx);
        vert.AddNeighbor(lastIdx);

        // Insert (MUST be after creation and linking)
        CLtoUse.push_back(vert);

        x += d;
        lastIdx = currentIdx;
    }
}
void Tunnel::CreateCenterline(int axis, bool flip)
{
    CreateCLHelper(centerline, axis, flip);
    CreateCLHelper(baseCenterline, axis, flip);
}

void Tunnel::Create()
{
    PreCreate();

    // Propagate center
    for (auto&& corner : corners)
    {
        corner.center = center;
    }

    Corner* corner;
    { // Right top front
        corner = &corners[0];
        corner->baseScale[0] = 1; corner->baseScale[1] = 1; corner->baseScale[2] = 1;
        corner->SetSurroundings(right, top, forward);
    }

    { // Right top back
        corner = &corners[1];
        corner->baseScale[0] = 1; corner->baseScale[1] = 1; corner->baseScale[2] = -1;
        corner->SetSurroundings(right, top, back);
    }

    { // Right bottom front
        corner = &corners[2];
        corner->baseScale[0] = 1; corner->baseScale[1] = -1; corner->baseScale[2] = 1;
        corner->SetSurroundings(right, bottom, forward);
    }
    
    { // Right bottom back
        corner = &corners[3];
        corner->baseScale[0] = 1; corner->baseScale[1] = -1; corner->baseScale[2] = -1;
        corner->SetSurroundings(right, bottom, back);
    }
    
    { // Left top front
        corner = &corners[4];
        corner->baseScale[0] = -1; corner->baseScale[1] = 1; corner->baseScale[2] = 1;
        corner->SetSurroundings(left, top, forward);
    }

    { // Left top back
        corner = &corners[5];
        corner->baseScale[0] = -1; corner->baseScale[1] = 1; corner->baseScale[2] = -1;
        corner->SetSurroundings(left, top, back);
    }

    { // Left bottom front
        corner = &corners[6];
        corner->baseScale[0] = -1; corner->baseScale[1] = -1; corner->baseScale[2] = 1;
        corner->SetSurroundings(left, bottom, forward);
    }
    
    { // Left bottom back
        corner = &corners[7];
        corner->baseScale[0] = -1; corner->baseScale[1] = -1; corner->baseScale[2] = -1;
        corner->SetSurroundings(left, bottom, back);
    }

    // Create vertices AFTER setting surroundings
    for (auto&& corner : corners)
    {
        corner.n = n;
        corner.radius = radius;
        corner.Create();
    }

    // Create centerlines
    centerline.push_back(Vertex(0));
    baseCenterline.push_back(Vertex(0));
    if (right)
    {
        CreateCenterline(0, false);
        endpointRight = centerline.size() - 1;
    }
    if (left)
    {
        CreateCenterline(0,  true);
        endpointLeft = centerline.size() - 1;
    }
    if (top)
    {
        CreateCenterline(1, false);
        endpointTop = centerline.size() - 1;
    }
    if (bottom)
    {
        CreateCenterline(1,  true);
        endpointBottom = centerline.size() - 1;
    }
    if (forward)
    {
        CreateCenterline(2, false);
        endpointForward = centerline.size() - 1;
    }
    if (back)
    {
        CreateCenterline(2,  true);
        endpointBack = centerline.size() - 1;
    }

    ErrCheck("Tunnel::Create");

    PostCreate();
}

void Tunnel::ApplyNoise()
{
    // Apply to each corner
    for (auto&& corner : corners)
    {
        corner.ApplyNoise();
    }

    // Apply to the centerline
    for (unsigned int i = 0; i < baseCenterline.size(); i++)
    {
        Vector3 p = noisePtr->getNoise(baseCenterline[i].coords + center);

        centerline[i].coords = baseCenterline[i].coords + p*Globals::tunnelNoiseScale;
    }

    // printf("%f, %f, %f\n", centerline[0].coords.x, centerline[0].coords.y, centerline[0].coords.z); // NORELEASE
}

void Tunnel::OldDraw() // NORELEASE
{
    // Arms
    {
        float cyl_height = 0.5f - radius*0.5;

        static Cylinder cyl;
        cyl.top = true;
        cyl.bottom = false;
        cyl.radius = radius * 0.5;
        cyl.height = cyl_height;
        cyl.SetTopCenter(0, 0.5f, 0);

        if (right)
        {
            glPushMatrix(); {
                glRotatef(-90, Z_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }

        if (left)
        {
            glPushMatrix(); {
                glRotatef(90, Z_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }

        if (top)
        {
                cyl.Draw();
        }

        if (bottom)
        {
            glPushMatrix(); {
                glRotatef(180, Z_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }

        if (forward)
        {
            glPushMatrix(); {
                glRotatef(90, X_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }

        if (back)
        {
            glPushMatrix(); {
                glRotatef(-90, X_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }
    }

    // Core
    static Cube core;
    core.radius = radius * 0.5;
    core.Draw();

}

void Tunnel::Draw(bool hasControl)
{
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);

        // OldDraw();

        for (auto &&corner : corners)
        {
            corner.Draw(hasControl);
        }
    } glPopMatrix();
}