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

#include <GL/glew.h>
#include <glfw3.h>
//#include <OpenGL/gl3.h>

// Include GLM
//#include <glm/glm.hpp>

//GLUT versions
//#include <GL/freeglut.h>
//#include <GL/glut.h>
#include <GLUT/glut.h>

//
#include "../FDTD_Classes/FDPlate.hpp"
#include "../common/shader.hpp"
//==============================================================================
// Global scope for these is not ideal
// consider bundling this and other methods in a class.

FDPlate plate;
float rot, rotx, roty, rotz;
float zoom = 0;
GLfloat currentEyeX = 45., currentEyeY = 45., currentEyeZ = 45.;

//==============================================================================
static const GLfloat g_vertex_buffer_data[] =
{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint programID;
//==============================================================================

/**
 Get the 3D normal of a triangular plane from it's three vertices

 @return the triangle normal as a pointer to a GLfloat array with 3 elements
 */
GLfloat* getTriNormal (const GLfloat* pointOne, const GLfloat* pointTwo, const GLfloat* pointThree)
{
    const GLfloat v1[3] = { pointTwo[0]-pointOne[0],
        pointTwo[1]-pointOne[1],
        pointTwo[1]-pointOne[0]};
    
    const GLfloat v2[3] = { pointThree[0]-pointOne[0],
        pointThree[1]-pointOne[1],
        pointThree[1]-pointOne[0]};
    
    static GLfloat triNorm[3] = { (v1[1]*v2[2] - v2[1]*v1[2]),
        -(v1[0]*v2[2] - v2[0]*v1[2]),
        (v1[0]*v2[1] - v2[0]*v1[1])};
    
    
    return triNorm;
    
}

//==============================================================================

void drawTriangle (const GLfloat* pointOne, const GLfloat* pointTwo, const GLfloat* pointThree)
{
    glBegin (GL_TRIANGLES);
    glVertex3fv (pointOne);
    glVertex3fv (pointTwo);
    glVertex3fv (pointThree);
    glEnd ();
}

/**
 Draw a Qaud outline from 4 points, moving clockwise or anti-clockwise
 */
void drawQuad (const GLfloat* pointOne, const GLfloat* pointTwo, const GLfloat* pointThree, const GLfloat* pointFour)
{
    glBegin (GL_LINE_LOOP);
    glVertex3fv (pointOne);
    glVertex3fv (pointTwo);
    glVertex3fv (pointThree);
    glVertex3fv (pointFour);
    glEnd();
}

/**
 Draw x, y, z Axis with a given length
 */
void drawAxes (const GLfloat length)
{
        glBegin (GL_LINES);
        //x
        glVertex3f (0., 0., 0.);
        glVertex3f (length, 0., 0.);
        //y
        glVertex3f (0., 0., 0.);
        glVertex3f (0., length, 0.);
        //z
        glVertex3f (0., 0., 0.);
        glVertex3f (0., 0., length);
        
        glEnd();
}

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
            
            break;
        case GLUT_KEY_DOWN:
            rot = 0;
            rotz = 0;
            rotx = 0;
            roty = 0;
            zoom = 0;
            break;
        case 119: // 'w' key
            zoom = (zoom < 0) ? 0 : .1;
            break;
        case 115: // 's' key
            zoom = (zoom > 0) ? 0 : -.1;
            break;
        case 27: // Escape key to Quit
            glutDestroyWindow ( glutGetWindow() );
            exit (0);
            break;
        case GLUT_KEY_F1: // F1 to restart
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
//    glDepthFunc(GL_LESS);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective (45.0, 1.0, 1.0, 250.0);
    gluLookAt ( currentEyeX,   currentEyeY, currentEyeZ,// eye
               0.0,   0.0,    0.0,  // centre
               0.0,   1.0,   0.0);  // up
    
    // Initialize GLEW
//    glewExperimental = GL_TRUE; // Needed for core profile
//    
//    GLuint vertexbuffer;
//    GLuint VertexArrayID;
//    glGenVertexArrays(1, &VertexArrayID);
//    glBindVertexArray(VertexArrayID);
//    
//    // set shader
//    const char vertShade[] = "/Users/admin/Documents/GitHub/SurfPlot/shaders/SimpleVertexShader.vertexshader";
//    const char fragShade[] = "/Users/admin/Documents/GitHub/SurfPlot/shaders/SimpleFragmentShader.fragmentshader";
//    programID = LoadShaders( vertShade, fragShade );
//    
//    glGenBuffers(1, &vertexbuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}

//==============================================================================
//==============================================================================
//==============================================================================

int main (int argc, char* argv[])
{
    plate.setup (44100, true);
    plate.setLoss(.2, .9);
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
//==============================================================================
//==============================================================================

void draw ()
{
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT );
    //==========================================================================
    //  Translation
    //==========================================================================
    glRotatef (rot, rotx, roty, rotz);
    glTranslatef(zoom, zoom, zoom);
    
    //==========================================================================
    //  Text
    //==========================================================================
    glColor4f(.7f, .7f, .7f, 0.0f);
    glRasterPos3f(-8.5, 20, 0);
    const unsigned char string[] = "Press F1 to Strike, esc to quit";
    for (const unsigned char* c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter (GLUT_BITMAP_8_BY_13, *c);
    }
    
    //==========================================================================
    //  Axis
    //==========================================================================
    glColor4f(.4f, .4f, .4f, .4f);
    drawAxes(10.);
    //==========================================================================
    
    //==========================================================================
    // none of this is ideal
    plate.updateScheme();
    const int xPointNum = plate.Nx;
    const int zPointNum = plate.Ny;
    const double* plateSurf = plate.u1;
    //==========================================================================
  
    glLineWidth(2.f);
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
            
            // Surface
            glColor4f((p1[1]*.05)+.5, 0, 1-(p1[1]*.02), 0);
            drawTriangle(p1, p2, p4);
            drawTriangle(p3, p2, p4);
            
            // Mesh
            glColor4f(.5, .5, .5, 0);
            glColor4f(.1, .1, .1, 0);
            drawQuad(p1, p2, p3, p4);
            
            // Normal
            const GLfloat* currentNorm1 = getTriNormal (p1, p2, p4);
            glNormal3fv(currentNorm1);
            const GLfloat* currentNorm2 = getTriNormal (p3, p2, p4);
            glNormal3fv(currentNorm2);
        }
    }
    
    
    // Use our shader
//    glUseProgram(programID);
//    
//    // first attribute buffer : vertices
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//    glVertexAttribPointer(
//                          0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                          3,        // size
//                          GL_FLOAT, // type
//                          GL_FALSE, // normalized?
//                          0,        // stride
//                          nullptr  // array buffer offset
//                          );
//    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
//    glDisableVertexAttribArray(0);
//    
    
    glutPostRedisplay ();
}

//==============================================================================

