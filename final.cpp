/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

// MainLib
#include "final.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "lighting.hpp"
#include "textures.hpp"

// Structures
#include "Colony.hpp"

// Noise
#include "Noise.hpp"
#include "Perlin.hpp"

// Menus
#include "Menus.hpp"

Model* displayModelPtr;
Colony colony;
Colony tunnel;
Colony chamber;
Colony allTunnels;

Noise* noisePtr;

OrbitLight* orbiterPtr;

// FIXME temp hack
Structure* currentStructure;
int currentCLidx;

int shader;

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

	// Textures
	// FIXME for SOME REASON having this load here makes the one inside textures.cpp work
	LoadTexBMP("Textures/dirtGroundTileable.bmp");
	InitTextures();
	BindTexture("dirt"); // FIXME

	// Lighting
	initLighting();
	Light ambient(0.3, 0, 0);
	// OrbitLight orbiter(0, 0.45, 0.7);
	OrbitLight orbiter(0, 0.8, 0);
	orbiter.radius = baseMag * 1.2;
	orbiterPtr = &orbiter;

	// Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// Face culling and Z-buffering
	glEnable(GL_CULL_FACE);
	if (Globals::viewMode == ViewMode::EXTERIOR)
		glCullFace(GL_BACK);
	else
		glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	ErrCheck("init");
}

void PopulateColony(Colony& colony)
{
	float tunnelCoords[][3] = {
		// Base layer
		{0, 0, 0},
		{0, 0, 1},
		{0, 0, 2},
		{1, 0, 2},

		// Up shaft
		{0, 1, 0},
		{0, 2, 0},
		{0, 3, 0},
		{-1, 3, 0},
	};

	unsigned char f = 1;
	unsigned char b = 2;
	unsigned char u = 4;
	unsigned char d = 8;
	unsigned char r = 16;
	unsigned char l = 32;

	unsigned char tunnelCon[] = {
		f+r+l+u,
		f+b+r,
		b+r+d,
		b+l,

		u+d,
		u+d+r,
		d+l,
		u+r,
	};

	float chamberCoords[][3] = {
		// Individual chambers
		{-1, 0, 0},
		{1, 2, 0},
		{0, -1, 2},

		// Grouped chambers
		{1, 0, 0},
		{2, 0, 0},
		{1, 0, 1},
		{2, 0, 1},
	};

	unsigned char chamberCon[] = {
		// Individual chambers
		r,
		l,
		u,

		// Grouped chambers
		r+l+f,
		l+f,
		r+l+f+b,
		l+b,
	};

	for (int i = 0; i < 8; i++)
	{
		colony.AddTunnel(tunnelCoords[i], tunnelCon[i]);
	}
	for (int i = 0; i < 7; i++)
	{
		colony.AddChamber(chamberCoords[i], chamberCon[i]);
	}

	colony.AddHill(-1, 4, 0);

	colony.center[1] = -1;
}

void PopulateTunnels(Colony& colony)
{
	float tunnelCoords[][3] = {
		// 0
		{0, 0, 0},

		// 1
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
		{-1, 0, 0},
		{0, -1, 0},
		{0, 0, -1},

		// 2 - x = 0
		{0, 1, 1},
		{0, -1, 1},
		{0, 1, -1},
		{0, -1, -1},

		// 2 - y = 0
		{1, 0, 1},
		{-1, 0, 1},
		{1, 0, -1},
		{-1, 0, -1},

		// 2 - z=0
		{1, 1, 0},
		{-1, 1, 0},
		{1, -1, 0},
		{-1, -1, 0},

		// 3 - all +
		{1, 1, 1},

		// 3 - one -
		{-1, 1, 1},
		{1, -1, 1},
		{1, 1, -1},

		// 3 - two -
		{-1, -1, 1},
		{-1, 1, -1},
		{1, -1, -1},

		// 3 - all -
		{-1, -1, -1},

		// FIXME doesn't include 2 tunnels per axis
	};

	unsigned char f = 1;
	unsigned char b = 2;
	unsigned char u = 4;
	unsigned char d = 8;
	unsigned char r = 16;
	unsigned char l = 32;

	unsigned char tunnelCon[] = {
		// 0
		0,

		// 1
		r,
		u,
		f,
		l,
		d,
		b,

		// 2 - x = 0
		u+f,
		d+f,
		u+b,
		d+b,

		// 2 - y = 0
		r+f,
		l+f,
		r+b,
		l+b,

		// 2 - z=0
		r+u,
		l+u,
		r+d,
		l+d,

		// 3 - all +
		r+u+f,

		// 3 - one -
		l+u+f,
		r+d+f,
		r+u+b,

		// 3 - two -
		l+d+f,
		l+u+b,
		r+d+b,

		// 3 - all -
		l+d+b,
	};

	for (int i = 0; i < 27; i++)
	{
		colony.AddTunnel(tunnelCoords[i], tunnelCon[i]);
	}
}


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void Cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   //  Front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,+1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
   glEnd();
   //  Back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

void display()
{
	preDisplay();

	switch(Globals::sceneChoice)
	{
	case Scene::colony:
	{
		baseMag = 4;
		displayModelPtr = &colony;
		break;
	}
	case Scene::tunnel:
	{
		baseMag = 1;
		displayModelPtr = &tunnel;
		break;
	}
	case Scene::chamber:
	{
		baseMag = 2;
		displayModelPtr = &chamber;
		break;
	}
	case Scene::allTunnels:
	{
		baseMag = 4;
		displayModelPtr = &allTunnels;
		break;
	}
	default:
		Fatal(999, "Unknown scene %d\n", Globals::sceneChoice);
	}

	// FIXME this should probably not be in display
	orbiterPtr->UpdatePosition();
	orbiterPtr->Draw();

	if (true)
	{
		glUseProgram(shader);

		//  For brick shader set "uniform" variables

		float time = 0.001*glutGet(GLUT_ELAPSED_TIME);
		int id = glGetUniformLocation(shader,"time");
		glUniform1f(id,time);
		id = glGetUniformLocation(shader,"ambientLight");
		glUniform1i(id,0); // FIXME don't hardcode
		id = glGetUniformLocation(shader,"directedLight");
		glUniform1i(id,1); // FIXME don't hardcode
	}

	if (true)
		displayModelPtr->Draw();
	else
		Cube(0,0,0,1,1,1,0);

	//  Revert to fixed pipeline
	glUseProgram(0);

	if (Toggles::Noise::showPVectors) noisePtr->DrawNoise();

	ErrCheck("display");

	postDisplay();
}

/*
 *  Read text file
 */
char* ReadText(const char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal(999, "Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal(999, "Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal(999, "Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj, const char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal(999, "Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal(999, "Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal(999, "Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal(999, "Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type, const char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(const char* VertFile, const char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

int main(int argc, char* argv[])
{
	Perlin noise;
	noisePtr = &noise;

	bool useNoise = true;
	Toggles::Noise::showPerturbed = useNoise;
	Toggles::Noise::showBase = !useNoise;
	float zero[] = {0, 0, 0}; // FIXME not sure why the 1-param ApplyNoise isn't getting inherited

	// Create the scene to be displayed
	Globals::sceneChoice = Scene::tunnel;

	// PopulateColony(colony);
	if (useNoise) colony.ApplyNoise(zero);

	// forward : 1
	//    back : 2
	//     top : 4
	//  bottom : 8
	//   right : 16
	//    left : 32
	tunnel.AddTunnel(0, 0, 0, 21); // Tunnel
	if (useNoise) tunnel.ApplyNoise(zero);

	// chamber.AddChamber(0, 0, 0, 21); // Chamber
	if (useNoise) chamber.ApplyNoise(zero);

	// PopulateTunnels(allTunnels);
	if (useNoise) allTunnels.ApplyNoise(zero);

	init(argc, argv);

	SetCallbacks();

	Menus::CreateMenus();

	currentStructure = tunnel.getChild(0);
	currentCLidx = 0;
	auto& CL = *(currentStructure->getCL());
	Globals::InteriorView::eyePos = CL[currentCLidx].coords;

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	// PrintMoveOptions();

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}