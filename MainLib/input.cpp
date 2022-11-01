
#include "input.hpp"
#include "window.hpp"

float mouse_x = windowWidth * 0.5f;
float mouse_y = windowHeight * 0.5f;

/*
 * Process standard keys
 */
// float speed = 0.5;
void key(unsigned char k, int x, int y)
{
	switch (k) {
    /*
	// Movement
	case 'w':
	case 'W':
		if (viewMode == VIEW_FIRST)
		{
			// Move along the line from eye to center
			fp_eyeX += speed * (fp_centX - fp_eyeX);
			fp_eyeY += speed * (fp_centY - fp_eyeY);
			fp_eyeZ += speed * (fp_centZ - fp_eyeZ);
		}
		else if (lightMode == LIGHTMODE_MANU)
		{
			// Move the light up
			lightPhi += 3*lightSpeed;
		}
		break;
	case 'a':
	case 'A':
		if (viewMode != VIEW_FIRST && lightMode == LIGHTMODE_MANU)
		{
			// Move the light clockwise
			lightTheta += 3*lightSpeed;
		}
		break;
	case 's':
	case 'S':
		if (viewMode == VIEW_FIRST)
		{
			// Move opposite the line from eye to center
			fp_eyeX -= speed * (fp_centX - fp_eyeX);
			fp_eyeY -= speed * (fp_centY - fp_eyeY);
			fp_eyeZ -= speed * (fp_centZ - fp_eyeZ);
		}
		else if (lightMode == LIGHTMODE_MANU)
		{
			// Move the light down
			lightPhi -= 3*lightSpeed;
		}
		break;
	case 'd':
	case 'D':
		if (viewMode != VIEW_FIRST && lightMode == LIGHTMODE_MANU)
		{
			// Move the light counterclockwise
			lightTheta -= 3*lightSpeed;
		}
		break;
	
	// Meta keys
	case 'm':
	case 'M':
		viewMode += 1;
		break;
	case 'l':
	case 'L':
		lightMode = 1 - lightMode;
		break;
	case '[':
		if (sceneMode == 0)
			// Unsigned char, so can't just wrap negatives
			sceneMode = SCENE_MAX;
		else
			sceneMode -= 1;
		break;
	case ']':
		sceneMode += 1;
		break;
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
	default:
		return;
	}
	
    /*
	// Clamp view mode
	if (viewMode > VIEW_FIRST) viewMode = VIEW_ORTHO;
	
	// Clamp scene mode
	if (sceneMode > SCENE_MAX && sceneMode != SCENE_CUBE) sceneMode = SCENE_MAIN;
	
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
void mouse(int x, int y)
{
	mouse_x = (float)x / windowWidth - 0.5;
	mouse_y = (float)y / windowHeight - 0.5;
}