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
#include <cstring>
#include "../FDTD_Classes/FDPlate.hpp"

//GLUT versions
#include <GL/freeglut.h>
//#include <GL/glut.h>
//#include <GLUT/glut.h>
//==============================================================================
// Global scope for these is not ideal
// consider bundling this and other methods in a class.

FDPlate plate;
float rot, rotx, roty, rotz;

//==============================================================================
static void key (int key, int x, int y)
{
    switch (key)
    {
            
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
            //            plate.setInitialCondition();
            plate.addStrike();
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
    glEnable(GL_DEPTH_TEST);
    draw ();
    glutSwapBuffers ();
}
//==============================================================================
void init ()
{
    glEnable (GL_DEPTH_TEST|GL_COLOR_MATERIAL);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective (45.0, 1.0, 1.0, 250.0);
    gluLookAt ( -45.0,   45.0, 45.0,// eye
               0.0,   0.0,    0.0,  // centre
               0.0,   1.0,   0.0);  // up
}

//==============================================================================
//==============================================================================
//==============================================================================

int main (int argc, char* argv[])
{
    plate.setup (44100, true);
    plate.setLoss(1, .9);
    plate.setInitialCondition();
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow ("Finite Difference Plate");
    glutDisplayFunc (display);

    glutSpecialFunc (key);
    init ();
//    PointLight(-45.5,45.5,45.5, .3, 10, 10);
    glutMainLoop ();
    return 0;
}

//==============================================================================
//==============================================================================
//==============================================================================
void draw ()
{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos3f(-8.5, 20, 0);
    
    const unsigned char string[] = "FDTD Thin Plate: Press F1 to Strike, esc to quit";
    for (const unsigned char* c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter (GLUT_BITMAP_8_BY_13, *c);
    }
    
    //==========================================================================
    //  Axis
    //==========================================================================
    
    glColor4f(.3f, .3f, .3f, 1.0f);
    glBegin (GL_LINES);
    //x
    glVertex3f (0., 0., 0.);
    glVertex3f (10., 0., 0.);
    //y
    glVertex3f (0., 0., 0.);
    glVertex3f (0., 10., 0.);
    //z
    glVertex3f (0., 0., 0.);
    glVertex3f (0., 0., 10.);
    
    glEnd();
    
    
    //==========================================================================
    
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
    
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glRotatef (rot, rotx, roty, rotz);
    const float amp =  5e4;
    for (int x = 0; x < xPointNum; ++x)
    {
        const float surfX = (float)x - ((float)xPointNum*.5);
        for (int z = 0; z < zPointNum; ++z)
        {
            const float surfZ = (float)z - ((float)zPointNum*.5);
            const int cp =  int(x + (z * zPointNum));

            // Mesh Points
            const GLfloat p1[3] = { surfX,
                                    static_cast<float>(amp*plateSurf[cp]),
                                    surfZ};
            const GLfloat p2[3] = { surfX,
                                    static_cast<float>(amp*plateSurf[cp+zPointNum]),
                                    surfZ+1};
            const GLfloat p3[3] = { surfX+1,
                                    static_cast<float>(amp*plateSurf[cp+zPointNum+1]),
                                    surfZ+1};
            const GLfloat p4[3] = { surfX+1,
                                    static_cast<float>(amp*plateSurf[cp+1]),
                                    surfZ};
            
            
            glColor4f(.15, .15, .15, .1);
            glBegin (GL_LINE_LOOP);
            glVertex3fv (p1);
            glVertex3fv (p2);
            glVertex3fv (p3);
            glVertex3fv (p4);
            glEnd ();
            
            glColor4f((p1[1]*.05)+.5, 0, 1-(p1[1]*.02), .5-(p1[1]*.02));
            glBegin (GL_QUADS);
            glVertex3fv (p1);
            glVertex3fv (p2);
            glVertex3fv (p3);
            glVertex3fv (p4);
            glEnd ();
            
            // Calculate Normals
            const GLfloat v1[3] = {p2[0]-p1[0],p2[1]-p1[1],p2[1]-p1[0]};
            const GLfloat v2[3] = {p4[0]-p3[0],p4[1]-p3[1],p4[1]-p3[0]};
            
            const GLfloat currentNorm[3] = { (v1[1]*v2[2] - v2[1]*v1[2]),
                -(v1[0]*v2[2] - v2[0]*v1[2]),
                (v1[0]*v2[1] - v2[0]*v1[1])};
            
            glNormal3fv(currentNorm);
        }
    }
    
    glutPostRedisplay ();
}

