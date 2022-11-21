
#include "input.hpp"
#include "globals.hpp"
#include "window.hpp"

/*
 * Process standard keys
 */
float speed = 0.05;
void fpsKey(unsigned char k)
{
	namespace iv = Globals::InteriorView;

	switch (k) {
	// Movement
	case 'w':
	case 'W':
		{
			// Move along the line from eye to center
			iv::eyePos[0] += speed * (iv::lookPos[0] - iv::eyePos[0]);
			iv::eyePos[1] += speed * (iv::lookPos[1] - iv::eyePos[1]);
			iv::eyePos[2] += speed * (iv::lookPos[2] - iv::eyePos[2]);
		}
		break;
	// case 'a':
	// case 'A':
	// 	// FIXME strafe
	// 	break;
	case 's':
	case 'S':
		{
			// Move opposite the line from eye to center
			iv::eyePos[0] -= speed * (iv::lookPos[0] - iv::eyePos[0]);
			iv::eyePos[1] -= speed * (iv::lookPos[1] - iv::eyePos[1]);
			iv::eyePos[2] -= speed * (iv::lookPos[2] - iv::eyePos[2]);
		}
		break;
	// case 'd':
	// case 'D':
	// 	// FIXME strafe
	// 	break;
	}
}
void key(unsigned char k, int x, int y)
{
	switch (k) {
	/*
	
	// Meta keys
	case 'm':
	case 'M':
		viewMode += 1;
		break;
	case 'l':
	case 'L':
		lightMode = 1 - lightMode;
		break;
	*/
	// FIXME no way to update the displayModelPtr, so scene doesn't actually change
	case '[':
		sceneChoice--;
		break;
	case ']':
		sceneChoice++;
		break;
	/*
	case '\\':
		sceneMode = SCENE_MAIN;
		break;
	*/
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

	fpsKey(k);
	
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
/*
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
	
	glutPostRedisplay();
}
*/


/*
 * Process mouse movement
 * Output is fraction of window
 */
void mouseMovement(int x, int y)
{
	Globals::mouse_x = (float)x / windowWidth - 0.5;
	Globals::mouse_y = (float)y / windowHeight - 0.5;

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
	// if (Globals::viewMode == ViewMode::INTERIOR) return;

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
	//glutSpecialFunc(special);
	glutPassiveMotionFunc(mouseMovement);
	glutMouseFunc(mouseAction);
}