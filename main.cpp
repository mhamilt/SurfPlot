//==============================================================================
//  C++ Surf Plot
//
//  Created by matthew hamilton on 15/10/2017.
//
//
//  A surf plot of the finite difference plate model class FDPlate
//
//  Note: GLUT is largely deprecated in macOS. This is really just for a short
//  demonstartion
//
//  TODO: Find a way to better organise to reduce the scope of variables and
//  avoid using global namespace.
//==============================================================================


#include <iostream>
#include <cmath>
#include <GLUT/glut.h>
#include "../FDTD_Classes/FDPlate.hpp"

//==============================================================================
// Global scope for these is not ideal
// consider bundling this and other methods in a class.

FDPlate plate;
float rot, rotx, roty, rotz;

//==============================================================================
static void key (int key, int x, int y)
{
    switch (key) {
            
        case GLUT_KEY_LEFT:
            rot = -1;
            rotx = 0;
            rotz = 0;
            roty = 1;
            break;
            
        case GLUT_KEY_RIGHT:
            rot = 1;
            roty = 1;
            rotz = 0;
            rotx = 0;
            break;
            
        case GLUT_KEY_UP:
            rot = 1;
            rotz = 0;
            rotx = 1;
            roty = 0;
            break;
        case GLUT_KEY_DOWN:
            rot = 0;
            rotz = 0;
            rotx = 0;
            roty = 0;
            break;
        case 27: // Escape key to Quit
            glutDestroyWindow ( glutGetWindow() );
            exit (0);
            break;
        case GLUT_KEY_F1: // F1 to restart
            plate.setInitialCondition();
            break;
    }
    glutPostRedisplay();
}

//==============================================================================
void draw ();
//==============================================================================
void display ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw ();
    glutSwapBuffers ();
}
//==============================================================================
void init ()
{
    
    glEnable (GL_DEPTH_TEST);
    glMatrixMode (GL_MODELVIEW);
    gluPerspective (45.0, 1.0, 1.0, 250.0);
    gluLookAt ( 45.0,   45.0, -40.0, // eye
               0.0,   0.0,    0.0, // centre
               0.0,   1.0,   0.0);// up
}
//==============================================================================
int main (int argc, char* argv[])
{
    plate.setup (44100, false);
    plate.setInitialCondition();
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow ("Finite Difference Plate");
    glutDisplayFunc (display);
    glutSpecialFunc (key);
    init ();
    glutMainLoop ();
    return 0;
}
//==============================================================================
void draw ()
{
    glRotatef (rot, rotx, roty, rotz);
    //==========================================================================
    // none of this is ideal
    plate.updateScheme();
    const int xPointNum = plate.Nx;
    const int zPointNum = plate.Ny;
    double* plateSurf = plate.u1;
    //==========================================================================
    
    // This code will loop over all points on the plate.
    // It creates a mesh by drawing adjacent points together (GL_LINE_STRIP)
    // TODO: use shading along with GL_QUADS to create a coloured surface
    
    // NOTE: centre of the window is (0,0,0) so X and Z must be adjusted
    // hence surfX and surfZ
    for (int x = 0; x < xPointNum-1; ++x)
    {
        const float surfX = (float)x - ((float)xPointNum*.5);
        for (int z = 0; z < zPointNum-1; ++z)
        {
            const float surfZ = (float)z - ((float)zPointNum*.5);
            const int cp =  int(x + (z * zPointNum));
            glBegin (GL_LINE_STRIP);
            glVertex3f (surfX, 1e5*plateSurf[cp], surfZ);
            glVertex3f (surfX, 1e5*plateSurf[cp+zPointNum], surfZ+1);
            glVertex3f (surfX+1, 1e5*plateSurf[cp+zPointNum+1], surfZ+1);
            glVertex3f (surfX+1, 1e5*plateSurf[cp+1], surfZ);
            glVertex3f (surfX, 1e5*plateSurf[cp], surfZ);
            glEnd ();
        }
    }

    glutPostRedisplay ();
}

