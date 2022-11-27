#include "Mine.hpp"
#include "globals.hpp"

void Mine::ApplyNoise(float offset[])
{
    // Get the Y-axis noise applied at the center
    float x = center[0] + offset[0];
    float y = center[1] + offset[1];
    float z = center[2] + offset[2];
    auto centerNoise = noisePtr->getNoise(x, y, z);

    float scale;
    bool useCenterNoise;
    for (unsigned int i = 0; i < baseVertices.size(); i++)
    {
		if (armIndices.find(i) == armIndices.end())
		{ // Main chamber
            scale = noiseScale;
            useCenterNoise = true;
		}
		else
		{ // Arm
			scale = Globals::tunnelNoiseScale;
            useCenterNoise = false;
		}


        std::vector<float> p;
        if (useCenterNoise)
        {
            p = centerNoise;
        }
        else
        {
            float x = baseVertices[i][0] + center[0] + offset[0];
            float y = baseVertices[i][1] + center[1] + offset[1];
            float z = baseVertices[i][2] + center[2] + offset[2];
            p = noisePtr->getNoise(x, y, z);
        }
        
		vertices[i][0] = baseVertices[i][0] + p[0]*scale;
        vertices[i][1] = baseVertices[i][1] + p[1]*scale;
        vertices[i][2] = baseVertices[i][2] + p[2]*scale;
    }
}