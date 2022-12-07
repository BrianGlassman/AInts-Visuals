#include "window.hpp"
#include "globals.hpp"
#include "input.hpp"

static GLint win = 0;

void CreateWindow()
{
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(windowWidth, windowHeight);
    win = glutCreateWindow("Brian Glassman - Graduate Final Project");

    ErrCheck("CreateWindow");
}

void Cleanup(void)
{
    glutDestroyWindow(win);
}

void Project()
{

    // Reference: CSCI 5229 Ex 9
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    switch (Globals::viewMode)
    {
    case ViewMode::INTERIOR:
        gluPerspective(60, (double)windowWidth / windowHeight, 0.01, 10);
        break;
    case ViewMode::EXTERIOR:
    default: // default to exterior (orthographic)
        //  Orthogonal projection box adjusted for the aspect ratio of the window
        {
            double asp = (windowHeight>0) ? (double)windowWidth/windowHeight : 1;
            float maxMag = baseMag * Globals::mouse_zoom;
            // glOrtho(left, right, bottom, top, near, far)
            glOrtho(-asp*maxMag, +asp*maxMag, -maxMag, +maxMag, -maxMag*1.5, +maxMag*1.5);
        }
        break;
    }
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
    
    ErrCheck("Project");
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    windowWidth = width;
    windowHeight = height;
    //  Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);

    Project();

    ErrCheck("reshape");
}