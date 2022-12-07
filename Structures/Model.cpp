#include "Model.hpp"
#include "globals.hpp"
#include "Noise.hpp"
#include "Shaders.hpp"

Model::Model()
{
    type = StructureType::Model;
}

Model::~Model() {}

void Model::PreCreate()
{
    vertices.clear();
    baseVertices.clear();
    normals.clear();
    baseNormals.clear();
    texCoords.clear();
    indices.clear();
}

// Should never be called, just an example to remember Pre/Post
void Model::Create()
{
    PreCreate();

    Fatal(999, "Model::Create called\n");

    PostCreate();
}

void Model::PostCreate()
{
    // Copy vertices to baseVertices
    for (auto&& vertex : vertices)
    {
        baseVertices.push_back(vertex);
    }

    // Normalize normals
    for (auto&& normal : normals)
    {
        normal.Normalize();
    }

    // Copy normals to baseNormals
    for (auto&& normal : normals)
    {
        baseNormals.push_back(normal);
    }

    // Set creation flag
    created = true;
}

void Model::ApplyNoiseHelper(unsigned int i, float scale)
{
    // Perturbation at the initial vertex
    Vector3 p = noisePtr->getNoise(baseVertices.at(i) + center);
    
    // --- Vertices ---
    vertices[i] = baseVertices.at(i) + p*scale;

    // --- Normals ---
    {
        auto& baseNormal = baseNormals.at(i);

        // Construct tangent vectors by crossing the normal with each axis
        Vector3 tx = baseNormal.Cross(Vector3::Right);
        Vector3 ty = baseNormal.Cross(Vector3::Up);
        Vector3 tz = baseNormal.Cross(Vector3::Forward);

        // Reduce magnitude to a small value
        tx = tx.Normalized() * 0.001;
        ty = ty.Normalized() * 0.001;
        tz = tz.Normalized() * 0.001;

        // Perturbation will be slightly different because of the offset
        Vector3 px = noisePtr->getNoise(baseVertices[i] + center + tx);
        Vector3 py = noisePtr->getNoise(baseVertices[i] + center + ty);
        Vector3 pz = noisePtr->getNoise(baseVertices[i] + center + tz);

        // Compute the perturbed tangent vectors
        tx = tx + px*scale - p*scale;
        ty = ty + py*scale - p*scale;
        tz = tz + pz*scale - p*scale;

        // Normalize for the sake of later computation
        tx.Normalize();
        ty.Normalize();
        tz.Normalize();

        // Eliminate zero-magnitude tangents
        bool xValid = tx.Magnitude() > 0;
        bool yValid = ty.Magnitude() > 0;
        bool zValid = tz.Magnitude() > 0;

        Vector3 tUse1, tUse2;
        if ((xValid + yValid + zValid) < 2) Fatal(999, "Not enough valid tangents\n");
        if (!zValid)
        {
            tUse1 = tx;
            tUse2 = ty;
        }
        else if (!yValid)
        {
            tUse1 = tx;
            tUse2 = tz;
        }
        else if (!xValid)
        {
            tUse1 = ty;
            tUse2 = tz;
        }
        else
        {
            // Use whichever two perturbed tangents are most orthogonal to each other
            float xDy = abs(tx.Dot(ty));
            float xDz = abs(tx.Dot(tz));
            float yDz = abs(ty.Dot(tz));
            if (xDy <= xDz && xDy <= yDz)
            {
                tUse1 = tx;
                tUse2 = ty;
            }
            else if (xDz <= xDy && xDz <= yDz)
            {
                tUse1 = tx;
                tUse2 = tz;
            }
            else if (yDz <= xDy && yDz <= xDz)
            {
                tUse1 = ty;
                tUse2 = tz;
            }
            else Fatal(999, "ApplyNoise failed somehow\n");
        }

        // Compute the new normal by crossing the perturbed tangents
        Vector3 normal = tUse1.Cross(tUse2);

        // Normalize
        normal.Normalize();

        // Assume normals never completely change direction, and any inversion is because of cross product ordering
        if (normal.Dot(baseNormal) < 0)
        {
            normal = normal.Reversed();
        }

        // Save the result
        normals[i] = normal;
    }
}

// Must be called AFTER Create so that both vertex lists are populated
void Model::ApplyNoise()
{
    for (unsigned int i = 0; i < baseVertices.size(); i++)
    {
        ApplyNoiseHelper(i, Globals::chamberNoiseScale);
    }
}

void Model::DrawNormalHelper(float scale, std::vector<Vector3> verticesToUse, std::vector<Vector3> normalsToUse)
{
    glBegin(GL_LINES);
    for (auto&& i : indices)
    {
        auto&& vertex = verticesToUse[i];
        auto&& normal = normalsToUse[i];
        glVertex3f(vertex.x, vertex.y, vertex.z);
        glVertex3f(scale * normal.x + vertex.x, scale * normal.y + vertex.y, scale * normal.z + vertex.z);
    }
    glEnd();
}
void Model::DrawNormals(float scale)
{
    PushShader(Shader::fixedPipeline);
    SetColor(1, 0, 0);
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);
        
        // Perturbed geometry
        if (Toggles::Noise::showPerturbed)
            DrawNormalHelper(scale, vertices, normals);
        
        if (Toggles::Noise::showBase)
        {
            if (Toggles::Noise::showPerturbed)
            {
                // Show base for comparison
                SetColor(0.7, 0.2, 0.2);
                glLineWidth(5);
                DrawNormalHelper(scale*0.5, vertices, baseNormals);
            }
            else
            {
                // Only show base
                DrawNormalHelper(scale, baseVertices, baseNormals);
            }
        }
    } glPopMatrix();
    glPopAttrib();
    SetColor(1, 1, 1);
    PopShader();
}

Cylinder::Cylinder()
{
}

void Cylinder::Draw(bool hasControl)
{
     glPushMatrix(); {
          glTranslatef(center[0], center[1], center[2]);
          glScalef(radius, height, radius);
          glTranslatef(0, -0.5, 0);

          // Top
          if (top)
          {
               glBegin(GL_TRIANGLE_FAN); {
                    glNormal3f(0, 1, 0);
                    glTexCoord2f(0.5, 0.5);
                    glVertex3d(0, 1, 0);
                    float x, z;
                    for (int theta=360; theta >= 0; theta -= (360 / n))
                    {
                         Polar2Cart(1, theta, &x, &z);
                         glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                         glVertex3d(x, 1, z);
                    }
               } glEnd();
          }

          // Sides
          if (sides)
          {
               glBegin(GL_QUAD_STRIP); {
                    float x, z;
                    for (int theta=0; theta <= 360; theta += (360 / n))
                    {
                         Polar2Cart(1, theta, &x, &z);
                         glNormal3f(x, 0, z);
                         glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                         glVertex3d(x, 0, z);
                         glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
                         glVertex3d(x, 1, z);
                    }
               } glEnd();
          }

          // Bottom
          if (bottom)
          {
               glBegin(GL_TRIANGLE_FAN); {
                    glNormal3f(0, -1, 0);
                glTexCoord2f(0.5, 0.5);
                    glVertex3d(0, 0, 0);
                    float x, z;
                    for (int theta=0; theta <= 360; theta += (360 / n))
                    {
                         Polar2Cart(1, theta, &x, &z);
                         glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                         glVertex3d(x, 0, z);
                    }
               } glEnd();
          }
     } glPopMatrix();
}

void Cylinder::SetTopCenter(float x, float y, float z)
{
     center[0] = x;
     center[1] = y - height / 2.0f;
     center[2] = z;
}

void Cylinder::SetBottomCenter(float x, float y, float z)
{
     center[0] = x;
     center[1] = y + height / 2.0f;
     center[2] = z;
}

Cube::Cube()
{
     sides = (
          left * 32 + right * 16 +
          bottom * 8 + top * 4 +
          back * 2 + forward * 1);
}

void Cube::Draw(bool hasControl)
{
    // Vertex Coordinates
    const float vertices[][3] = {
        {0, 0, 0}, // 0 - left bottom back
        {1, 0, 0}, // 1 - right bottom back
        {1, 1, 0}, // 2 - right top back
        {1, 0, 1}, // 3 - right bottom front
        {1, 1, 1}, // 4 - right top front
        {0, 1, 0}, // 5 - left top back
        {0, 1, 1}, // 6 - left top front
        {0, 0, 1}, // 7 - left bottom front
    };
    
    // Indices
    const unsigned char indices[][4] = {
        {7, 6, 5, 0}, // X = 0
        {1, 2, 4, 3}, // X = 1
        {0, 1, 3, 7}, // Y = 0
        {5, 6, 4, 2}, // Y = 1
        {5, 2, 1, 0}, // Z = 0
        {3, 4, 6, 7}, // Z = 1
    };
    
    glPushMatrix(); {
        // Apply transformations
        glTranslatef(center[0], center[1], center[2]); // Translate to chosen center
        glScalef(radius*2, radius*2, radius*2); // Apply scaling
        glTranslatef(-0.5, -0.5, -0.5); // Center at target coords
        
        for (int i=0; i<6; i++)
        {
            if (sides & (1 << i))
            {
                DrawLitQuad(
                    vertices[indices[5-i][0]],
                    vertices[indices[5-i][1]],
                    vertices[indices[5-i][2]],
                    vertices[indices[5-i][3]]
                );
            }
        }
    } glPopMatrix();
}

Sphere::Sphere()
{
}

void Sphere::Draw(bool hasControl)
{
    glPushMatrix(); {
        // Apply transformations
        glTranslatef(center[0], center[1], center[2]); // Translate to chosen center
        glScalef(radius, radius, radius); // Apply scaling

        int d = 360 / n;
        float x, z, r0, r1, y0, y1;

        // Draw sphere, using degenerate quads for the caps (ref Ex. 8)
        for (float phi = -90; phi < 90; phi += d)
        {
            Polar2Cart(1, phi, &r0, &y0);
            Polar2Cart(1, phi + d, &r1, &y1);

            glBegin(GL_QUAD_STRIP);
            for (int theta = 0; theta <= 360; theta += d)
            {
                Polar2Cart(r0, theta, &x, &z);
                glNormal3d(x, y0, z);
                glVertex3d(x, y0, z);
                Polar2Cart(r1, theta, &x, &z);
                glNormal3d(x, y1, z);
                glVertex3d(x, y1, z);
            }
            glEnd();
        }
    } glPopMatrix();
}