#include <iostream>

#include "Noise.hpp"
#include "Perlin.hpp"

#include "CSCIx229.h"
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "window.hpp"

Noise* noisePtr;

//-----------------------
// Function declarations
//-----------------------
static void init(int argc, char* argv[]);
void display();
void SetCallbacks()
{
	glutDisplayFunc(display);
	//FIXME
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
            
            vertices[j][0] += perturbation[0] * 0.1;
            vertices[j][1] += perturbation[1] * 0.1;
            vertices[j][2] += perturbation[2] * 0.45;
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

    const float sideLength = 0.5;
    cube.radius = 0.5 * sideLength;
    float mn = -1.5 * sideLength, mx = 1.5 * sideLength;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

    // Draw the baseline (grid without noise)
    glColor4f(1, 1, 1, 0.5);
    for (float x = mn; x <= mx; x += sideLength)
    {
        for (float y = mn; y <= mx; y += sideLength)
        {
            for (float z = mn; z <= mx; z += sideLength)
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
    for (float x = mn; x <= mx; x += sideLength)
    {
        for (float y = mn; y <= mx; y += sideLength)
        {
            for (float z = mn; z <= mx; z += sideLength)
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

    noisePtr = new Perlin;

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
    delete noisePtr;
	return 99;
}