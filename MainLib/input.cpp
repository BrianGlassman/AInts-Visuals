
#include "input.hpp"
#include "globals.hpp"
#include "window.hpp"


float speed = 1.0; // FIXME for now speed is irrelevant because movement is vertex-to-vertex

void PrintMoveOptions()
{
	printf("%d can move to ", currentCLidx);
	for (int neighbor : (*currentStructure->getCL())[currentCLidx].neighbors)
	{
		printf("%d, ", neighbor);
	}
	printf("\n");
}

void fpsMove(Vector3 goalMotion)
{
	auto& current = (*currentStructure->getCL())[currentCLidx];

	// Get all neighboring CL vertices
	std::vector<Vertex> neighbors;
	for (auto&& neighbor : current.neighbors)
	{
		neighbors.push_back((*currentStructure->getCL())[neighbor]);
	}

	// Get unit vectors pointing towards each neighbor
	std::vector<Vector3> directions;
	for (auto&& neighbor : neighbors)
	{
		Vector3 direction;
		direction.x = neighbor.coords.x - current.coords.x;
		direction.y = neighbor.coords.y - current.coords.y;
		direction.z = neighbor.coords.z - current.coords.z;
		direction.Normalize();
		directions.push_back(direction);
	}

	// Pick whichever vertex is best aligned with goalMotion
	int bestNew = currentCLidx;
	float bestMag = 0.75; // Have to be pointing mostly that way to count
	for (unsigned int i = 0; i < neighbors.size(); i++)
	{
		auto&& direction = directions[i];
		auto&& neighbor = neighbors[i];

		float dotMag = direction.Dot(goalMotion);
		if (Toggles::printMove) printf("%f towards %d, ", dotMag, neighbor.idx);
		if (dotMag > bestMag)
		{
			bestMag = dotMag;
			bestNew = neighbor.idx;
		}
	}
	if (Toggles::printMove) printf("\n");

	// Move to the chosen vertex
	currentCLidx = bestNew;
	Globals::InteriorView::eyePos = (*currentStructure->getCL())[bestNew].coords;

	if (Toggles::printMove) PrintMoveOptions();
}
void fpsKey(unsigned char k)
{
	namespace iv = Globals::InteriorView;

	switch (k) {
	// Movement
	case 'w':
	case 'W':
		{
			// Move along the line from eye to center
			Vector3 goalMotion;
			goalMotion.x = speed * iv::lookDir[0];
			goalMotion.y = speed * iv::lookDir[1];
			goalMotion.z = speed * iv::lookDir[2];
			fpsMove(goalMotion);
		}
		break;
	case 'a':
	case 'A':
		{
			// Move opposite Right vector
			Vector3 goalMotion;
			goalMotion.x = -speed * iv::right[0];
			goalMotion.y = -speed * iv::right[1];
			goalMotion.z = -speed * iv::right[2];
			fpsMove(goalMotion);
		}
		break;
	case 's':
	case 'S':
		{
			// Move opposite the line from eye to center
			Vector3 goalMotion;
			goalMotion.x = -speed * iv::lookDir[0];
			goalMotion.y = -speed * iv::lookDir[1];
			goalMotion.z = -speed * iv::lookDir[2];
			fpsMove(goalMotion);
		}
		break;
	case 'd':
	case 'D':
		{
			// Move along Right vector
			Vector3 goalMotion;
			goalMotion.x = speed * iv::right[0];
			goalMotion.y = speed * iv::right[1];
			goalMotion.z = speed * iv::right[2];
			fpsMove(goalMotion);
		}
		break;
	}
}

/*
 * Process standard keys
 */
void key(unsigned char k, int x, int y)
{
	switch (k) {
	// Meta keys
	case 'm':
	case 'M':
		if (Globals::viewMode == ViewMode::INTERIOR)
		{ // Change to exterior
			Globals::viewMode = ViewMode::EXTERIOR;
			glCullFace(GL_BACK);
			Project();
		}
		else
		{ // Change to interior
			Globals::viewMode = ViewMode::INTERIOR;
			glCullFace(GL_FRONT);
			Project();
		}
		break;
	/*
	case 'l':
	case 'L':
		lightMode = 1 - lightMode;
		break;
	*/
	case '[':
		Globals::sceneChoice--;
		break;
	case ']':
		Globals::sceneChoice++;
		break;
	case '\\':
		Globals::sceneChoice = Scene::colony;
		break;
	case 27: // Escape - exit
		Cleanup();
		fprintf(stdout, "User exit\n");
		exit(0);
		break;
	/*
	case 32: // Space - reset view
		if (viewMode == VIEW_FIRST)
		{
			fp_rotH = 0.0; fp_rotV = 0.0;
			fp_centX = 0.0; fp_centY = 0.0; fp_centZ = 0.0;
			fp_eyeX = 0.0; fp_eyeY = 0.0; fp_eyeZ = 20.0;
		}
		else
		{
			view_rotx = 45.0;
			view_roty = 40.0;
			view_rotz = 0.0;
		}
		break;
	*/
	}

	if (Globals::viewMode == ViewMode::INTERIOR) fpsKey(k);

	if (Globals::viewMode == ViewMode::EXTERIOR)
	{
		Input::buildKey(k);
	}
	
	/*
	// Clamp light position
	if (lightPhi > 89) lightPhi = 89;
	if (lightPhi < -89) lightPhi = -89;
	
	reshape(_width, _height);
	
	glutPostRedisplay();
	*/
}

/*
 * Processes special keys
 */
void special(int k, int x, int y)
{
	switch(k) {
	case GLUT_KEY_UP:
		key('w', x, y);
		break;
	case GLUT_KEY_LEFT:
		key('a', x, y);
		break;
	case GLUT_KEY_DOWN:
		key('s', x, y);
		break;
	case GLUT_KEY_RIGHT:
		key('d', x, y);
		break;
	default:
		return;
	}
	
	// glutPostRedisplay();
}


/*
 * Process mouse movement
 * Output is fraction of window
 */
void mouseMovement(int x, int y)
{
	Globals::mouse_x = (float)x / windowWidth - 0.5;
	Globals::mouse_y = (float)y / windowHeight - 0.5;

	// Create a deadzone in the middle
	if (Globals::mouse_x > -0.1 && Globals::mouse_x < 0.1) Globals::mouse_x = 0;
	if (Globals::mouse_y > -0.1 && Globals::mouse_y < 0.1) Globals::mouse_y = 0;

	// DEBUG fprintf(stdout, "(%f, %f) = %d / %d - 0.5, %d / %d - 0.5\n", mouse_x, mouse_y, x, windowWidth, y, windowHeight);
}

/*
 * Process mouse buttons and scroll wheel
 * References:
 *   https://community.khronos.org/t/processing-mouse-wheel-actions/55776/3
 *   https://www.opengl.org/resources/libraries/glut/spec3/node50.html
 */
float zoomSpeed = 0.05;
void mouseAction(int button, int state, int x, int y)
{
	// Ignore mouse wheel in interior view
	if (Globals::viewMode == ViewMode::INTERIOR) return;

	switch(button)
	{
	case 3: // Wheel down - zoom out
		Globals::mouse_zoom -= zoomSpeed;
		break;
	case 4: // Wheel up - zoom in
		Globals::mouse_zoom += zoomSpeed;
		break;
	default:
		return;
	}

	// FIXME
	if (Globals::mouse_zoom < 0.1)
	{
		Globals::mouse_zoom = 0.1;
	}

	Project();
}

void SetInputCallbacks()
{
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutPassiveMotionFunc(mouseMovement);
	glutMouseFunc(mouseAction);
}