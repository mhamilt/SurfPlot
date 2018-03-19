#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// GLUT
#include <GL/glew.h>
#include <GLUT/glut.h>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
