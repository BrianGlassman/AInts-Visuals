#include "Corner.hpp"
#include "globals.hpp"
#include "Shaders.hpp"
#include "Structure.hpp"

Corner::Corner()
{
}

void Corner::SetRotateAndScale()
{
    // Base meshes are defined with X >= Y >= Z 
    // So rotate to match that convention

    if (surroundings.x >= surroundings.y && surroundings.x >= surroundings.z)
    {
        if (surroundings.y >= surroundings.z)
        {
            // x >= y >= z - Matches default
            meshRotate[0] = 0; meshRotate[1] = 0; meshRotate[2] = 0;
            meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
            windCW = false;
        }
        else
        {
            // x >= z >= y - Switch Y and Z
            meshRotate[0] = 90; meshRotate[1] = 0; meshRotate[2] = 0;
            meshScale[0] = 1; meshScale[1] = -1; meshScale[2] = 1;
            windCW = true;
        }
    }
    else if (surroundings.y >= surroundings.x && surroundings.y >= surroundings.z)
    {
        if (surroundings.z >= surroundings.x)
        {
            // y >= z >= x - X --> Z, Y --> X, Z --> Y
            meshRotate[0] = 0; meshRotate[1] = 90; meshRotate[2] = 90;
            meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
            windCW = false;
        }
    }
    else if (surroundings.z >= surroundings.x && surroundings.z >= surroundings.y)
    {
        if (surroundings.x >= surroundings.y)
        {
            // z >= x >= y - X --> Y, Y --> Z, Z --> X
            meshRotate[0] = -90; meshRotate[1] = 0; meshRotate[2] = -90;
            meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
            windCW = false;
        }
    }
    else
    {
        Fatal(999, "SetRotateAndScale fell through");
    }

    // Mirroring reverses the face culling behavior, so have to correct for that
    if (baseScale[0]*baseScale[1]*baseScale[2] < 0)
    {
        windCW = not windCW;
    }

    if (windCW)
        windMode = GL_CW;
    else
        windMode = GL_CCW;
}

void Corner::CreateClosed()
{
    int d = 360 / n;
    float r, y;
    float x, z;

    // Quarter sphere, using degenerate quads for the caps
    int rowLength = n / 4 + 1;
    int numRows = rowLength - 1; // Pole doesn't count as a row
    int poleIdx = vertices.size();

    // Generate the vertices, starting from the pole
    normals.push_back({0, radius, 0});
    vertices.push_back({0, radius, 0});
    for (float phi = 90 - d; phi >= 0; phi -= d)
    {
        Polar2Cart(radius, phi, &r, &y);
        for (int theta = 0; theta <= 90; theta += d)
        {
            Polar2Cart(r, theta, &z, &x); // Flip X/Z to get CCW winding
            normals.push_back({x, y, z});
            vertices.push_back({x, y, z});
        }
    }

    // Assign indices for first row, all using the pole
    for (int col = 0; col < rowLength; col++)
    {
        indices.push_back(poleIdx);
        indices.push_back(poleIdx + 1 + col);
    }
    indexBounds.push_back(indices.size());

    // Assign indices for the rest of the rows
    int thisRowStart, nextRowStart;
    for (int row = 0; row < numRows - 1; row++)
    {
        thisRowStart = poleIdx+1 + rowLength*row;
        nextRowStart = thisRowStart + rowLength;
        for (int col = 0; col < rowLength; col++)
        {
            indices.push_back(thisRowStart + col);
            indices.push_back(nextRowStart + col);
        }
        indexBounds.push_back(indices.size());
    }
}
/// @brief Creates the outer point of the core
/// @param i0 axis index along the arm
/// @param i1 secondary axis index
/// @param c1 secondary coordinate
/// @param i2 tertiary axis index
/// @param c2 tertiary coordinate
void Corner::CreateCoreOuter(int i0, int i1, float c1, int i2, float c2)
{
    Vector3 coords;
    coords[i0] = 0; coords[i1] = c1; coords[i2] = c2;
    indices.push_back(vertices.size());
    normals.push_back(coords);
    coords[i0] = radius;
    vertices.push_back(coords);
}
void Corner::CreateArm(int i0, int i1, int i2, bool reverse)
{
    float d = (0.5 - radius) / panels;

    Vector3 coords;
    float x = radius, y, z;
    for (int i = 0; i < panels; i++)
    {
        indexBounds.push_back(indices.size());
        for (float theta = 90; theta <= 180; theta += (360 / n))
        {
            // Top-left quadrant, moving CCW
            Polar2Cart(radius, theta, &z, &y);
            z = -z;
            coords[i0] = 0; coords[i1] = y; coords[i2] = z;

            // Outer edge (Inner if reversed)
            indices.push_back(vertices.size());
            normals.push_back(coords);
            coords[i0] = reverse ? x : x + d;
            vertices.push_back(coords);

            // Inner edge (Outer if reversed)
            coords[i0] = 0;
            indices.push_back(vertices.size());
            normals.push_back(coords);
            coords[i0] = reverse ? x + d : x;
            vertices.push_back(coords);
        }

        x += d;
    }
}
void Corner::XTunnel(bool makeY, bool makeZ)
{
    float x, y, z;
    for (float theta = 90; theta <= 180; theta += (360 / n))
    {
        // Top-left quadrant when viewed from +X, moving CCW
        Polar2Cart(radius, theta, &z, &y);
        z = -z;

        // Outer point
        CreateCoreOuter(0, 1, y, 2, z);

        // Inner point
        indices.push_back(vertices.size());
        if (makeY)
        {
            if (makeZ)
            { // X, Y, and Z tunnels - stop at X = Y or X = Z
                if (y >= z)
                {
                    x = y;
                }
                else
                {
                    x = z;
                }
            }
            else
            { // X and Y tunnels - stop at X = Y
                x = y;
            }
        }
        else
        { // Only X tunnel - Stop at X = 0
            x = 0;
        }
        normals.push_back({0, y, z});
        vertices.push_back({x, y, z});
    }

    CreateArm(0, 1, 2, false);
}
void Corner::YTunnel(bool makeZ)
{
    indexBounds.push_back(indices.size());
    
    float x, z;
    for (int theta = 270; theta >= 180; theta -= (360 / n))
    {
        // Bottom-left quadrant when viewed from +Y, moving Clockwise
        Polar2Cart(radius, theta, &z, &x);
        x = -x; z = -z;

        // Inner point
        if (makeZ && x < z)
        { // Y = Z
            float y = z;
            indices.push_back(vertices.size());
            normals.push_back({x, 0, z});
            vertices.push_back({x, y, z});
        }
        else
        { // Y = X
            float y = x;
            indices.push_back(vertices.size());
            normals.push_back({x, 0, z});
            vertices.push_back({x, y, z});
        }

        // Outer point
        CreateCoreOuter(1, 0, x, 2, z);
    }

    CreateArm(1, 0, 2, true);
}
void Corner::ZTunnel()
{
    indexBounds.push_back(indices.size());

    float x, y, z;
    for (int theta = 90; theta >= 0; theta -= (360 / n))
    {
        // Top-right quadrant when viewed from +Z, moving Clockwise
        Polar2Cart(radius, theta, &x, &y);

        // Inner point Z=X or Z=Y
        z = (x >= y) ? x : y;
        indices.push_back(vertices.size());
        normals.push_back({x, y, 0});
        vertices.push_back({x, y, z});

        // Outer point(s)
        CreateCoreOuter(2, 0, x, 1, y);
    }

    CreateArm(2, 1, 0, true);
}

void Corner::Create()
{
    PreCreate();
    indexBounds.clear();
    indexBounds.push_back(0);

    SetRotateAndScale();

    switch(surroundings.sqrMagnitude())
    {
    case 0:
        CreateClosed();
        break;
    case 1:
        XTunnel(false, false);
        break;
    case 2:
        XTunnel(true, false);
        YTunnel(false);
        break;
    case 3:
        XTunnel(true, true);
        YTunnel(true);
        ZTunnel();
        break;
    default:
        Fatal(999, "Unknown connection %d\n", surroundings.sqrMagnitude());
    }

    // Apply scaling and rotation to the created vertices
    for (auto&& vertex : vertices)
    {
        // fprintf(stdout, "Pre (%f, %f, %f) rot (%f, %f, %f) * (%f, %f, %f)\n", // NORELEASE
        //     vertex[0], vertex[1], vertex[2], // NORELEASE
        //     meshRotate[0], meshRotate[1], meshRotate[2], // NORELEASE
        //     meshScale[0], meshScale[1], meshScale[2]); // NORELEASE

        // Rotate around Z axis
        vertex.RotateZ(meshRotate[2]);
        // fprintf(stdout, "Post Z (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]); // NORELEASE

        // Rotate around Y axis
        vertex.RotateY(meshRotate[1]);
        // fprintf(stdout, "Post Y (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]); // NORELEASE

        // Rotate around X axis
        vertex.RotateX(meshRotate[0]);
        // fprintf(stdout, "Post X (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]); // NORELEASE

        // Scale
        vertex[0] *= baseScale[0] * meshScale[0];
        vertex[1] *= baseScale[1] * meshScale[1];
        vertex[2] *= baseScale[2] * meshScale[2];

        // fprintf(stdout, "Post scale (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]); // NORELEASE
    }

    // Apply scaling and rotation to the created normals
    for (auto&& normal : normals)
    {
        normal.RotateZ(meshRotate[2]);
        normal.RotateY(meshRotate[1]);
        normal.RotateX(meshRotate[0]);

        normal[0] *= baseScale[0] * meshScale[0];
        normal[1] *= baseScale[1] * meshScale[1];
        normal[2] *= baseScale[2] * meshScale[2];
    }

    indexBounds.push_back(indices.size());
    PostCreate();
}

void Corner::DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals)
{
    glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
        // Convert vector of vectors to flat array
        float vertexArray[drawVertices.size() * 3];
        for (unsigned int i = 0; i < drawVertices.size(); i++)
        {
            vertexArray[i*3 + 0] = drawVertices[i][0];
            vertexArray[i*3 + 1] = drawVertices[i][1];
            vertexArray[i*3 + 2] = drawVertices[i][2];
            // fprintf(stdout, "%d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]); // NORELEASE
        }
        float normalArray[drawNormals.size() * 3];
        for (unsigned int i = 0; i < drawNormals.size(); i++)
        {
            normalArray[i*3 + 0] = drawNormals[i][0];
            normalArray[i*3 + 1] = drawNormals[i][1];
            normalArray[i*3 + 2] = drawNormals[i][2];
        }

        unsigned int indexArray[indices.size()];
        for (unsigned int i = 0; i < indices.size(); i++)
        {
            indexArray[i] = indices[i];
            // fprintf(stdout, "%d\n", indices[i]); // NORELEASE
        }

        glVertexPointer(3, GL_FLOAT, 0, vertexArray);
        glNormalPointer(GL_FLOAT, 0, normalArray);
        for (unsigned int i = 0; i < indexBounds.size() - 1; i++)
        {
            int count = indexBounds[i+1] - indexBounds[i];
            glDrawElements(GL_QUAD_STRIP, count, GL_UNSIGNED_INT, indexArray + indexBounds[i] * sizeof(unsigned char));
        }
    } glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
}
void Corner::Draw(bool hasControl)
{
    if (Toggles::showNormals)
    {
        // Counteract the center-translation, since that already happens in Tunnel
        glPushMatrix(); {
            glTranslatef(-center[0], -center[1], -center[2]);
            DrawNormals(0.25);
        } glPopMatrix();
    }

    if (hasControl)
    {
        if (Toggles::debug)
        {
            PushShader(Shader::fixedPipeline);
            glPushAttrib(GL_ENABLE_BIT); glDisable(GL_TEXTURE_2D);
            SetColor(0.75 + baseScale[0]*.25, 0.75 + baseScale[1]*.25, 0.75 + baseScale[2]*.25);
        }
        else
        {
            PushShader(Shader::threeDshader);
        }
    }

    glFrontFace(windMode);

    glPushMatrix(); {
        // Intentionally don't translate by center. Already gets handled in Tunnel

        // Perturbed geometry
        if (Toggles::Noise::showPerturbed)
            DrawHelper(vertices, normals);

        // Baseline geometry
        if (Toggles::Noise::showBase)
        {
            if (Toggles::Noise::showPerturbed && hasControl)
            { // Use white, transparent wireframe
                SetColor(1, 1, 1, 0.5);
                glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
                glDisable(GL_TEXTURE_2D);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } // else: Use colored, textured geometry
            DrawHelper(baseVertices, baseNormals);
            if (Toggles::Noise::showPerturbed && hasControl)
            {
                glPopAttrib();
                SetColor(1, 1, 1, 1);
            }
        }
    } glPopMatrix();

    if (Toggles::debug && hasControl)
    {
        glPopAttrib();
        SetColor(1, 1, 1);
    }
    
    glFrontFace(GL_CCW);
    if (hasControl) PopShader();

    ErrCheck("Corner::Draw");
}
