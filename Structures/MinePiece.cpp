#include "MinePiece.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

MinePiece::MinePiece(std::shared_ptr<OBJ> OBJtoUse)
{
    type = StructureType::MinePiece;
    OBJptr = OBJtoUse;
}

void MinePiece::PreCreate()
{
    Chamber::PreCreate();
    OBJindices.clear();
    OBJcolors.clear();
}

void MinePiece::Create(bool joinToChamber)
{
    if (OBJptr == nullptr) return;

    PreCreate();

    if (joinToChamber)
    {
        // Create Chamber arms/caps so they can be connected to
        Chamber::Create(false, false, true);
    }

    // Need the color vector to be the same length as the other vectors for the sake of indexing
    Vector3 grey({0.5, 0.5, 0.5});
    for (std::size_t i = 0; i < vertices.size(); i++)
    {
        OBJcolors.push_back(grey);
    }

    // Copy over OBJ values
    // NOTE: OBJ reuses vertices (I think?), I do not
    for (auto& face : OBJptr->OBJfaces)
    {
        for (int i = 0; i < 3; i ++)
        {
            OBJindices.push_back(vertices.size());
            vertices.push_back(OBJptr->OBJvertices[face.vIdxs[i]]);
            normals.push_back(OBJptr->OBJnormals[face.nIdxs[i]]);
            OBJcolors.push_back(OBJptr->OBJcolors[face.cIdxs[i]]);
        }
    }

    if (joinToChamber) { // Join the OBJ geometry to the Chamber geometry
        float mx = 0.43; // Large enough that it only triggers for Core
        std::vector<int> edge;
        for (int axis = 0; axis <= 2; axis++)
        {
            // --- Find the furthest points
            float val;
            for (std::size_t i = OBJindices.front() ; i < vertices.size(); i++)
            {
                auto& vertex = vertices.at(i);
                val = abs(vertex[axis]);

                if (val < mx) continue;

                if (val > mx)
                {
                    edge.clear();
                    mx = val;
                }

                if (val == mx)
                {
                    edge.push_back(i);
                }
            }
        }

        // --- Move the edge vertices to the nearest Chamber arm vertices
        for (auto& i : edge)
        {
            auto& vertex = vertices.at(i);
            auto& normal = normals.at(i);

            float bestSqrMag = 999;
            int bestI = i;
            Vector3 bestCoords;
            for (int ci = 0; ci < OBJindices.front(); ci++)
            {
                auto& cvert = vertices.at(ci);
                if ((cvert - vertex).SqrMag() < bestSqrMag)
                {
                    bestSqrMag = (cvert - vertex).SqrMag();
                    bestI = ci;
                }
            }
            vertex = vertices.at(bestI);
            normal = normals.at(bestI);
        }
    }

    PostCreate();

    ErrCheck("MinePiece::Create");
}

void MinePiece::DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals)
{
    glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); glEnableClientState(GL_COLOR_ARRAY); {
        // Convert vector of vectors to flat array
        float vertexArray[drawVertices.size() * 3];
        for (unsigned int i = 0; i < drawVertices.size(); i++)
        {
            vertexArray[i*3 + 0] = drawVertices[i][0];
            vertexArray[i*3 + 1] = drawVertices[i][1];
            vertexArray[i*3 + 2] = drawVertices[i][2];
        }
        float normalArray[drawNormals.size() * 3];
        for (unsigned int i = 0; i < drawNormals.size(); i++)
        {
            normalArray[i*3 + 0] = drawNormals[i][0];
            normalArray[i*3 + 1] = drawNormals[i][1];
            normalArray[i*3 + 2] = drawNormals[i][2];
        }

        // colorArray only used for OBJ vertices
        float colorArray[OBJcolors.size() * 3];
        for (unsigned int i = 0; i < OBJcolors.size(); i++)
        {
            colorArray[i*3 + 0] = OBJcolors[i][0];
            colorArray[i*3 + 1] = OBJcolors[i][1];
            colorArray[i*3 + 2] = OBJcolors[i][2];
        }

        if (OBJindices.size() > 0)
        { // Draw with GL_TRIANGLES
            unsigned int triIndexArray[OBJindices.size()];
            for (unsigned int i = 0; i < OBJindices.size(); i++)
            {
                triIndexArray[i] = OBJindices[i];
            }

            glVertexPointer(3, GL_FLOAT, 0, vertexArray);
            glNormalPointer(GL_FLOAT, 0, normalArray);
            glColorPointer(3, GL_FLOAT, 0, colorArray);
            glDrawElements(GL_TRIANGLES, OBJindices.size(), GL_UNSIGNED_INT, triIndexArray);
        }
    } glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_COLOR_ARRAY);

    ErrCheck("MinePiece::DrawHelper");
}

void MinePiece::Draw(bool hasControl)
{
    if (OBJptr == nullptr) return;

    if (hasControl) PushShader(Shader::threeDshader);
    glPushMatrix(); {
        glTranslatef(center.x, center.y, center.z);

        // Perturbed geometry
        if (Toggles::Noise::showPerturbed) DrawHelper(vertices, normals);
        // Only show base if not showing perturbed, too cluttered to show both
        else if (Toggles::Noise::showBase) DrawHelper(baseVertices, baseNormals);

    } glPopMatrix();
    if (hasControl) PopShader();

    ErrCheck("MinePiece::Draw");
}