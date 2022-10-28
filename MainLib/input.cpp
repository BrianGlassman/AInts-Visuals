#include "CSCIx229.h"

static void key(unsigned char k, int x, int y)
{
	switch (k) {
	case 27: /* Escape - exit*/
		glutDestroyWindow(win);
		fprintf(stdout, "User exit\n");
		exit(0);
		break;
	default:
		return;
	}
	
	glutPostRedisplay();
}