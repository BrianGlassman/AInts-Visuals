#include <iostream>

#include "Noise.hpp"
#include "Perlin.hpp"

#include "CSCIx229.h"
#include "util.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "window.hpp"

Perlin* noisePtr;

//-----------------------
// Function declarations
//-----------------------
static void init(int argc, char* argv[]);
void display();
void SetCallbacks()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);

	SetInputCallbacks();

	ErrCheck("SetCallbacks");
}

static void init(int argc, char* argv[])
{
	// Do all the OpenGL setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// Window
	CreateWindow();

	#ifdef USEGLEW
		if (glewInit() != GLEW_OK)
		{
			Fatal(1, "Error initializing GLEW\n");
		}
	#endif

	// Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// Face culling and Z-buffering
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	ErrCheck("init");
}

const static float baseVertices[][3] = {
    {0, 0, 0}, // 0 - left bottom back
    {1, 0, 0}, // 1 - right bottom back
    {1, 1, 0}, // 2 - right top back
    {1, 0, 1}, // 3 - right bottom front
    {1, 1, 1}, // 4 - right top front
    {0, 1, 0}, // 5 - left top back
    {0, 1, 1}, // 6 - left top front
    {0, 0, 1}, // 7 - left bottom front
};

const static unsigned char indices[][4] = {
    {7, 6, 5, 0}, // X = 0
    {1, 2, 4, 3}, // X = 1
    {0, 1, 3, 7}, // Y = 0
    {5, 6, 4, 2}, // Y = 1
    {5, 2, 1, 0}, // Z = 0
    {3, 4, 6, 7}, // Z = 1
};

void DrawPerturbedCube(float sideLength, float xCenter, float yCenter, float zCenter)
{
    float x, y, z;
    float vertices[4][3];
    // Iterate through sides
    for (int i = 0; i < 6; i++)
    {
        // Iterate through vertices on this side
        for (int j = 0; j < 4; j++)
        {
            // Cube vertices are centered around (0.5, 0.5, 0.5)
            vertices[j][0] = xCenter + sideLength*(baseVertices[indices[5-i][j]][0] - 0.5);
            vertices[j][1] = yCenter + sideLength*(baseVertices[indices[5-i][j]][1] - 0.5);
            vertices[j][2] = zCenter + sideLength*(baseVertices[indices[5-i][j]][2] - 0.5);

            // Perturb
            auto perturbation = noisePtr->getNoise(
                vertices[j][0],
                vertices[j][1],
                vertices[j][2]);
            
            // Limit perturbation to less than half a side to prevent intersections
            vertices[j][0] += perturbation[0] * sideLength * 0.49;
            vertices[j][1] += perturbation[1] * sideLength * 0.49;
            vertices[j][2] += perturbation[2] * sideLength * 0.49;
        }

        DrawLitQuad(vertices[0],
                    vertices[1],
                    vertices[2],
                    vertices[3]);
    }
}

void display()
{
	preDisplay();

    static Cube cube;

    const float sideLength = 1 / 3.0;
    cube.radius = 0.5 * sideLength;
    // Extents of the grid
    float mn = -1, mx = 1;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

    // Draw the perturbations
    glColor4f(1.0, 1.0, 0.6, 0.5);
    for (float x = floor(mn); x <= ceil(mx); x++)
    {
        for (float y = floor(mn); y <= ceil(mx); y++)
        {
            for (float z = floor(mn); z <= ceil(mx); z++)
            {
                glPushAttrib(GL_POINT_BIT);
                glPointSize(15);
                glBegin(GL_POINTS);
                glVertex3f(x, y, z);
                glEnd();
                glPopAttrib();

                auto p = noisePtr->getPVector(x, y, z);
                glBegin(GL_LINES);
                glVertex3f(x, y, z);
                glVertex3f(x + p.x * (sideLength * 0.5), y + p.y * (sideLength * 0.5), z + p.z * (sideLength * 0.5));
                glEnd();
            }
        }
    }

    //--- Change from grid-extents to center locations ---
    mn += sideLength * 0.5; mx -= sideLength * 0.5;
    //---   ---

    // Draw the baseline (grid without noise)
    glColor4f(1, 1, 1, 0.5);
    for (float x = mn; x <= mx + 0.01; x += sideLength)
    {
        for (float y = mn; y <= mx + 0.01; y += sideLength)
        {
            for (float z = mn; z <= mx + 0.01; z += sideLength)
            {
                glPushMatrix();
                glTranslatef(x, y, z);
                cube.Draw();
                glPopMatrix();
            }
        }
    }

    // Draw the perturbed grid
    glColor4f(0.6, 1.0, 0.6, 1.0);
    for (float x = mn; x <= mx + 0.01; x += sideLength)
    {
        for (float y = mn; y <= mx + 0.01; y += sideLength)
        {
            for (float z = mn; z <= mx + 0.01; z += sideLength)
            {
                DrawPerturbedCube(sideLength, x, y, z);
            }
        }
    }

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	postDisplay(2);
}

int main(int argc, char* argv[])
{
    init(argc, argv);

    SetCallbacks();

    noisePtr = new Perlin();

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
    delete noisePtr;
	return 99;
}